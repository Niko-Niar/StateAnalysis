//
// Created by kulakov on 06.04.2021.
//

#include "ClickhouseDB.h"

#include <nlohmann/json.hpp>
#include "../ordebug.h"

using namespace clickhouse;
using namespace optirepair;

void ClickhouseDB::init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) {
    m_cluster = cluster;
    m_callback = callback;

    nlohmann::json cfg = nlohmann::json::parse(config);
    ClientOptions options;

    if (cfg.contains("host") && !cfg.value("host", "").empty()) {
        options.SetHost(cfg["host"]);
    } else {
        options.SetHost("localhost");
    }

    // порт не ставить, т.к. ведет к зависанию. См.: https://issue.life/questions/57496295

    if (cfg.contains("login") && !cfg.value("login", "").empty()) {
        options.SetUser(cfg["login"]);
    }

    if (cfg.contains("password") && !cfg.value("password", "").empty()) {
        options.SetPassword(cfg["password"]);
    }

    options.SetPingBeforeQuery(true);

    delete m_client;
    try {
        m_client = new Client(options);
    } catch (const std::exception &ex) {
        ordebug::warning(__PRETTY_FUNCTION__, __LINE__, ex.what());
        if (m_callback != nullptr) {
            m_callback->databaseGotError(this, ex.what());
        }
    }

}

void ClickhouseDB::updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    throw std::runtime_error("ClickhouseDB::updateStatus() NOT IMPLEMENTED!!! status=" + status);
}

ModuleDescription ClickhouseDB::getServiceName(const std::string &serviceID) {
    throw std::runtime_error("ClickhouseDB::getServiceName() NOT IMPLEMENTED!!! serviceID=" + serviceID);
}

void ClickhouseDB::storeError(const std::string &data) {
    throw std::runtime_error("ClickhouseDB::storeError() NOT IMPLEMENTED!!! data=" + data);
}

void ClickhouseDB::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    throw std::runtime_error("ClickhouseDB::sendConfig() NOT IMPLEMENTED!!! config=" + config);
}

void ClickhouseDB::updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    throw std::runtime_error("ClickhouseDB::updateConfigField() NOT IMPLEMENTED!!!");
}

void ClickhouseDB::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    throw std::runtime_error("ClickhouseDB::getConfig() NOT IMPLEMENTED!!!");
}

std::string ClickhouseDB::storeData(long long int timestamp, bool useLevel, int level,
                                    const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data) {
    if (m_client == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to ClickHouse server");
    }

    Block b;

    auto c_time = std::make_shared<clickhouse::ColumnInt64>();
    c_time->Append(timestamp);
    b.AppendColumn("timestamp", c_time);

    if (useLevel) {
        auto c_level = std::make_shared<clickhouse::ColumnInt32>();
        c_level->Append(level);
        b.AppendColumn("level", c_level);
    }

    auto items = data->getItems();

    for (const auto& key : items) {
        if (key == "timestamp" || (useLevel && key == "level")) {
            continue;
        }

        auto itemType = data->getItemType(key);
        switch (itemType) {
            case OR_TYPE_STRING:
            case OR_TYPE_JSON: {
                auto c_item = std::make_shared<ColumnString>();
                c_item->Append(data->getString(key));
                b.AppendColumn(key, c_item);
                break;
            }
            case OR_TYPE_DOUBLE: {
                auto c_item = std::make_shared<ColumnFloat64>();
                c_item->Append(data->getDouble(key));
                b.AppendColumn(key, c_item);
                break;
            }
            case OR_TYPE_FLOAT: {
                auto c_item = std::make_shared<ColumnFloat32>();
                c_item->Append(data->getFloat(key));
                b.AppendColumn(key, c_item);
                break;
            }
            case OR_TYPE_LONG: {
                auto c_item = std::make_shared<ColumnInt64>();
                c_item->Append(data->getLong(key));
                b.AppendColumn(key, c_item);
                break;
            }
            case OR_TYPE_INT: {
                auto c_item = std::make_shared<ColumnInt32>();
                c_item->Append(data->getInt(key));
                b.AppendColumn(key, c_item);
                break;
            }
            case OR_TYPE_BOOL: {
                auto c_item = std::make_shared<ColumnUInt8>();
                c_item->Append(data->getBool(key) ? 1 : 0);
                b.AppendColumn(key, c_item);
                break;
            }
            case OR_TYPE_ARRAY: {
                auto ca_item = std::make_shared<clickhouse::ColumnArray>(std::make_shared<clickhouse::ColumnFloat32>());
                auto c_item = std::make_shared<clickhouse::ColumnFloat32>();
                auto values = data->getArray(key);
                for(auto item : values) {
                    c_item->Append(static_cast<const float>(item));
                }
                ca_item->AppendAsColumn(c_item);
                b.AppendColumn(key, ca_item);
                break;
            }
            default:
                throw std::runtime_error("ClickhouseDB::storeData(): UNKNOWN TYPE " + std::to_string(itemType) + " for key " + key);
        }
    }

    // запись в БД
    try {
        m_client->Insert(m_cluster + ".\"" + senderId + "_" + type + "\"", b);
    } catch (std::system_error &ex) {
        ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, ex.what());
        try {
            m_client->ResetConnection();
            createTableIfNotExists(m_cluster + ".\"" + senderId + "_" + type + "\"", useLevel, senderType, senderId, data);
            m_client->Insert(m_cluster + ".\"" + senderId + "_" + type + "\"", b);
        } catch (std::exception &ex2) {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("After ") + ex.what() + " try to reset connection and got " + ex2.what());
            if (m_callback != nullptr) {
                m_callback->databaseGotError(this, std::string("After ") + ex.what() + " try to reset connection and got " + ex2.what());
            }
        }
    } catch (std::exception &ex) {
        try {
            createTableIfNotExists(m_cluster + ".\"" + senderId + "_" + type + "\"", useLevel, senderType, senderId, data);
            m_client->Insert(m_cluster + ".\"" + senderId + "_" + type + "\"", b);
        } catch (std::exception &ex) {
            ordebug::error(__PRETTY_FUNCTION__, __LINE__, ex.what());
            if (m_callback != nullptr) {
                m_callback->databaseGotError(this, std::string("Got error during data insertion: ") + ex.what());
            }
        }
    }

    return {};
}

void ClickhouseDB::createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) {
    std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " ( `timestamp` Int64 ";
    if (isEventTable) {
        query += ", `level` Int32 ";
    }
    auto items = block->getItems();
    for (const auto& item : items) {
        if (item == "timestamp" || (isEventTable && item == "level")) {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "DataBlock includes system field " + item + ", this field was skipped");
            continue;
        }
        auto itemType = block->getItemType(item);
        query += ", `" + item + "` ";
        switch (itemType) {
            case OR_TYPE_ARRAY:
                query += "Array(Float32)";
                break;
            case OR_TYPE_BOOL:
                query += "UInt8";
                break;
            case OR_TYPE_INT:
                query += "Int32";
                break;
            case OR_TYPE_FLOAT:
                query += "Float32";
                break;
            case OR_TYPE_LONG:
                query += "Int64";
                break;
            case OR_TYPE_JSON:
            case OR_TYPE_STRING:
                query += "String";
                break;
            case OR_TYPE_DOUBLE:
                query += "Float64";
                break;
            default:
                throw std::runtime_error("ClickhouseDB::createTableIfNotExists(): UNKNOWN TYPE " +std::to_string(itemType) + " FOR KEY " + item );
        }
    }

    query += ") ENGINE = MergeTree PARTITION BY toYYYYMM(toDate(timestamp)) ORDER BY (timestamp) SETTINGS index_granularity = 8192";
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, query);
    try {
        m_client->Execute(query);
    } catch (const std::exception& ex) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, ex.what());
    }
}

void ClickhouseDB::getConfigsByType(const std::string &dataType, const DataBlock &block) {
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__ ) + " NOT IMPLEMENTED!!!");
}

void ClickhouseDB::requestData(DataQuery pQuery) {
    // 1. Подготовка запроса
    auto blockItems = pQuery.getBlock()->getItems();
    std::string query;
    query = "SELECT timestamp";

    for (const auto &item: blockItems) {
        query += ", " + item;
    }

    query += " FROM `" + m_cluster + "`.`" + pQuery.getModuleId() + "_" + pQuery.getType() + "`";

    bool isFirst = true;
    if (pQuery.getTimestamp() > 0) {
        query += " WHERE timestamp=" + std::to_string(pQuery.getTimestamp());
        isFirst = false;
    }

    auto conds = pQuery.getFilter();
    if (!conds.empty()) {
        auto condData = nlohmann::json::parse(conds);
        for (auto &item : condData.items()) {
            if (isFirst) {
                query += " WHERE";
                isFirst = false;
            } else {
                query += " AND";
            }

            if (item.value().is_number_integer()) {
                query += " \"" + item.key() + "\"=" + std::to_string(item.value().get<int>()) + "";
            } else if (item.value().is_number_unsigned()) {
                query += " \"" + item.key() + "\"=" + std::to_string(item.value().get<unsigned int>()) + "";
            } else if (item.value().is_number_float()) {
                query += " \"" + item.key() + "\"=" + std::to_string(item.value().get<float>()) + "";
            } else {
                query += " \"" + item.key() + "\"=\"" + to_string(item.value()) + "\"";
            }
        }
    }

    if (pQuery.getTimestamp() == 0) {
        query += " ORDER BY timestamp DESC";
    }

    query += " LIMIT 1";

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, query);

    // 2. Оправка запроса в БД
    auto cb = this->m_callback;
    try {
        m_client->Select(query, [pQuery, cb] (const clickhouse::Block &b) {
            auto result = ClickhouseDB::parseClickhouseBlock(b, pQuery.getBlock());
            if (cb && result)
                cb->databaseGotData(pQuery, result);
        });
    } catch (std::system_error &ex) {
        m_client->ResetConnection();
        try{
            m_client->Select(query, [pQuery, cb] (const clickhouse::Block &b) {
                auto result = ClickhouseDB::parseClickhouseBlock(b, pQuery.getBlock());
                if (cb && result)
                    cb->databaseGotData(pQuery, result);
            });
        } catch (std::exception &ex2) {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("After \"") + ex.what() + "\" try to reset connection and got " + ex2.what());
            if (cb) {
                cb->databaseGotError(this, std::string("After \"") + ex.what() + "\" try to reset connection and got " + ex2.what());
            }
        }
    } catch (std::exception &ex) {
        ordebug::error(__PRETTY_FUNCTION__, __LINE__, ex.what());
    }
}

DataBlock *ClickhouseDB::parseClickhouseBlock(const Block &b, DataBlock *provider) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Parse " + std::to_string(b.GetRowCount()) + " rows, " +
            std::to_string(b.GetColumnCount()) + " columns");

    //TODO: переделать на обработку массива
    if (b.GetRowCount() > 1) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Got more than 1 record, parse first");
    }

    for (size_t rowIndex = 0; rowIndex < b.GetRowCount(); rowIndex++) {
        auto ret = provider->createInstance();

        for (size_t columnIndex = 0; columnIndex < b.GetColumnCount(); columnIndex++) {
            const auto& columnName = b.GetColumnName(columnIndex);
            if (columnName == "timestamp") {
                ret->setBlockTimestamp(b[columnIndex]->As<clickhouse::ColumnInt64>()->At(rowIndex));
                continue;
            }
            switch (ret->getItemType(columnName)) {
                case OR_TYPE_STRING:
                case OR_TYPE_JSON: {
                    ret->setValue(columnName, b[columnIndex]->As<clickhouse::ColumnString>()->At(rowIndex));
                    break;
                }
                case OR_TYPE_INT: {
                    ret->setValue(columnName, b[columnIndex]->As<clickhouse::ColumnInt32>()->At(rowIndex));
                    break;
                }
                case OR_TYPE_LONG: {
                    ret->setValue(columnName, b[columnIndex]->As<clickhouse::ColumnInt64>()->At(rowIndex));
                    break;
                }
                case OR_TYPE_FLOAT: {
                    ret->setValue(columnName, b[columnIndex]->As<clickhouse::ColumnFloat32>()->At(rowIndex));
                    break;
                }
                case OR_TYPE_DOUBLE: {
                    ret->setValue(columnName, b[columnIndex]->As<clickhouse::ColumnFloat64>()->At(rowIndex));
                    break;
                }
                case OR_TYPE_ARRAY: {
                    std::vector<double> data;
                    auto col = b[columnIndex]->As<clickhouse::ColumnArray>()->GetAsColumn(rowIndex);
                    data.resize(col->Size());
                    for (size_t i = 0; i < col->Size(); ++i) {
                        data[i] = ((double) (*col->As<clickhouse::ColumnFloat32>())[i]);
                    }
                    ret->setValue(columnName, data);
                    break;
                }
                case OR_TYPE_BOOL: {
                    ret->setValue(columnName, b[columnIndex]->As<clickhouse::ColumnInt8>()->At(rowIndex) != 0);
                    break;
                }
                default:
                    throw std::runtime_error(std::string(__PRETTY_FUNCTION__ ) + " TYPE NOT IMPLEMENTED!!!");
            }
        }
        return ret;
    }

    return nullptr;
}

std::vector<std::string> ClickhouseDB::getServiceList() {
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

int ClickhouseDB::removeData(std::string serviceId, std::string messageType, long long int timestamp) {
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void ClickhouseDB::checkStatus() {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "NOT IMPLEMENTED");
}
