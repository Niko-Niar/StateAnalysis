//
// Created by kulakov on 01.07.2021.
//

#include <nlohmann/json.hpp>
#include "SqliteDB.h"

#include "../ordebug.h"

using namespace optirepair;

void SqliteDB::updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    if (m_db == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

ModuleDescription SqliteDB::getServiceName(const std::string &serviceID) {
    if (m_db == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void SqliteDB::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    if (m_db == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void SqliteDB::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    if (m_db == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

std::string SqliteDB::storeData(long long int timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type,
                         DataBlock *data) {
    if (m_db == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected");
    }

    std::string tableName = senderId + "_" + type;

    std::string query = "INSERT INTO \"" + std::string(tableName) + "\" (`timestamp`";

    if (useLevel)
        query += ", `level`";

    auto itemList = data->getItems();
    std::string values;

    for (const auto& field : itemList) {
        if (field == "timestamp" || (useLevel && field == "level")) {
            continue;
        }
        query += ", `" + field + "`";
        values += ", ?";
    }

    query += ") VALUES (" + std::to_string(timestamp) + (useLevel ?  ", " + std::to_string(level) : "") + values + ")";

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, query);

    sqlite3_stmt *prep_stmt = nullptr;
    int scode = -1;

    scode = sqlite3_prepare(this->m_db, query.c_str(), -1, &prep_stmt, nullptr);
    if (scode != SQLITE_OK) {
        createTableIfNotExists(tableName, useLevel, senderType, senderId, data);
        scode = sqlite3_prepare(this->m_db, query.c_str(), -1, &prep_stmt, nullptr);
        if (scode != SQLITE_OK) {
            auto msg = std::string("Prepare error: ") + sqlite3_errmsg(this->m_db);
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, msg);
            if (m_callback) {
                m_callback->databaseGotError(this, msg);
            }
            return {};
        }
    }

    int index = 1;
    for (const auto& item : itemList) {
        if (item == "timestamp" || (useLevel && item == "level")) {
            continue;
        }
        auto itemType = data->getItemType(item);

        std::string value;

        try{
            switch (itemType) {
                case OR_TYPE_BOOL: {
                    scode = sqlite3_bind_int(prep_stmt, index, data->getBool(item) ? 1 : 0);
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=int; item=" + item + "; value=" + std::to_string(data->getInt(item)));
                    }
                    break;
                }
                case OR_TYPE_INT: {
                    scode = sqlite3_bind_int(prep_stmt, index, data->getInt(item));
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=int; item=" + item + "; value=" + std::to_string(data->getInt(item)));
                    }
                    break;
                }
                case OR_TYPE_FLOAT: {
                    scode = sqlite3_bind_double(prep_stmt, index, data->getFloat(item));
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=float; item=" + item + "; value=" + std::to_string(data->getFloat(item)));
                    }
                    break;
                }
                case OR_TYPE_DOUBLE: {
                    scode = sqlite3_bind_double(prep_stmt, index, data->getDouble(item));
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=double; item=" + item + "; value=" + std::to_string(data->getDouble(item)));
                    }
                    break;
                }
                case OR_TYPE_LONG: {
                    scode = sqlite3_bind_int64(prep_stmt,index,data->getLong(item));
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=long; item=" + item + "; value=" + std::to_string(data->getLong(item)));
                    }
                    break;
                }
                case OR_TYPE_STRING: {
                    value = data->getString(item);
                    scode = sqlite3_bind_text(prep_stmt, index, value.c_str(), strlen(value.c_str()), SQLITE_TRANSIENT);
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=string; item=" + item + "; value=" + value);
                    }
                    break;
                }
                case OR_TYPE_JSON: {
                    value = data->getString(item);
                    scode = sqlite3_bind_text(prep_stmt, index, value.c_str(), strlen(value.c_str()), SQLITE_TRANSIENT);
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=json; item=" + item + "; value=" + value);
                    }
                    break;
                }
                case OR_TYPE_ARRAY: {
                    value = data->getString(item);
                    scode = sqlite3_bind_blob(prep_stmt, index, value.c_str(), strlen(value.c_str()), SQLITE_TRANSIENT);
                    if (scode != SQLITE_OK) {
                        throw std::runtime_error("type=array; item=" + item + "; value=" + value);
                    }
                    break;
                }
                default:
                    throw std::runtime_error("storeData(): type " + std::to_string(itemType) + " not implemented!!!");
            }
            index++;
        } catch (const std::exception &ex) {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, ex.what() + std::string(": ") + sqlite3_errmsg(this->m_db));
            if (m_callback) {
                m_callback->databaseGotError(this, std::string("Error during insert data: ") + ex.what() + std::string(": ") + sqlite3_errmsg(this->m_db));
            }
            sqlite3_finalize(prep_stmt);
            return {};
        }
    }

    scode = sqlite3_step(prep_stmt);
    if (scode != SQLITE_DONE) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Error during insert data: ") + sqlite3_errmsg(this->m_db) + "; " +
                sqlite3_errstr(scode) + "; code=" + std::to_string(scode));
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("Error during insert data: ") + sqlite3_errmsg(this->m_db));
        }
        sqlite3_finalize(prep_stmt);
        return {};
    }

    sqlite3_finalize(prep_stmt);

    return std::to_string(sqlite3_last_insert_rowid(this->m_db));
}

void SqliteDB::storeError(const std::string &data) {
    if (m_db == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void SqliteDB::init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) {
    std::ignore = moduleType;
    // сохраняем конфиг
    m_cluster = cluster;
    m_callback = callback;

    nlohmann::json cfg = nlohmann::json::parse(config);
    if (cfg.contains("path")) {
        m_path = cfg["path"];
    } else {
        m_path = ".";
    }

    auto db_path = m_path + "/" + serviceId + ".sqlite";

    // подключаемся к БД
    int rc;
    rc = sqlite3_open(db_path.c_str(), &m_db);
    if( rc ) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__,  std::string ("Can't open database: ") + sqlite3_errmsg(m_db));
        throw std::runtime_error("Can't open database");
    } else {
        ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Opened database successfully\n");
    }

    // установка журналирования в асинхронном режиме
    char *err = nullptr;

    sqlite3_exec(m_db, "PRAGMA journal_mode=WAL;", [](void *unused, int count, char **data, char **columns) {
        std::ignore = unused;
        std::ignore = data;
        std::ignore = columns;
        ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "count=" + std::to_string(count));
        return 0;
    }, nullptr, &err);

    if (err != nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, err);
    }

    // включение ожидания освобождения БД 100 мс
    sqlite3_busy_timeout(m_db, 100);
    sqlite3_busy_handler(m_db, SqliteDB::busy_callback, nullptr);
}

void SqliteDB::updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock * block) {
    std::ignore = moduleType;
    std::ignore = serviceId;
    std::ignore = block;

    if (m_db == nullptr) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void SqliteDB::createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) {
    std::ignore = senderType;
    std::ignore = senderId;
    if (m_db == nullptr) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Can't execute operation: Client not connected");
    }

    std::string query = "CREATE TABLE IF NOT EXISTS \"" + tableName + "\" ( `id` INTEGER PRIMARY KEY, `timestamp` INTEGER ";
    if (isEventTable) {
        query += ", `level` INTEGER ";
    }
    auto items = block->getItems();
    for (const auto& item : items) {
        if (item == "id" || item == "timestamp" || (isEventTable && item == "level")) {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "DataBlock includes system field " + item + ", this field was skipped");
            continue;
        }
        auto itemType = block->getItemType(item);
        query += ", `" + item + "` ";
        switch (itemType) {
            case OR_TYPE_ARRAY:
                query += "BLOB";
                break;
            case OR_TYPE_BOOL:
            case OR_TYPE_INT:
            case OR_TYPE_LONG:
                query += "INTEGER";
                break;
            case OR_TYPE_FLOAT:
            case OR_TYPE_DOUBLE:
                query += "REAL";
                break;
            case OR_TYPE_JSON:
            case OR_TYPE_STRING:
                query += "TEXT";
                break;
            default:
                throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": UNKNOWN TYPE " +std::to_string(itemType) + " FOR KEY " + item );
        }
    }

    query += ")";
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, query);
    sqlite3_stmt *prep_stmt = nullptr;
    int scode = -1;

    scode = sqlite3_prepare(this->m_db, query.c_str(), -1, &prep_stmt, nullptr);
    if (scode != SQLITE_OK) {
        auto msg = std::string("Prepare error: ") + sqlite3_errmsg(this->m_db);
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, msg);
        return;
    }

    scode = sqlite3_step(prep_stmt);
    if (scode != SQLITE_DONE) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Error during insert table: ") + sqlite3_errmsg(this->m_db));
    }

    sqlite3_finalize(prep_stmt);
}

void SqliteDB::getConfigsByType(const std::string &dataType, const DataBlock &block) {
    std::ignore = dataType;
    std::ignore = block;
    if (m_db == nullptr) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

int SqliteDB::busy_callback(void * stmt, int times) {
    std::ignore = stmt;
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Passed busy for " + std::to_string(times) + " times");
    if (times < 5)
        return 1;
    return 0;
}

void SqliteDB::requestData(DataQuery pQuery) {
    std::ignore = pQuery;
    if (m_db == nullptr) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));

}

std::vector<std::string> SqliteDB::getServiceList() {
    if (m_db == nullptr) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

int SqliteDB::removeData(std::string serviceId, std::string messageType, long long int timestamp) {
    std::ignore = serviceId;
    std::ignore = messageType;
    std::ignore = timestamp;

    if (m_db == nullptr) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Can't execute operation: Client not connected");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void SqliteDB::checkStatus() {
    if (m_db == nullptr) {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": Can't execute operation: Client not connected");
    }
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "NOT IMPLEMENTED");
}
