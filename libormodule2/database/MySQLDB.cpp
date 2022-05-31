//
// Created by kulakov on 14.04.2021.
//

#include "MySQLDB.h"

#include <stdexcept>
#include <nlohmann/json.hpp>

#include "../ordebug.h"

using namespace optirepair;

using namespace nlohmann;

MySQLDB::~MySQLDB() {
    if (m_connection) {
        mysql_close(m_connection);
    }
}

void MySQLDB::init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) {
    m_callback = callback;

    json cfg = json::parse(config);

    std::string host;
    if (cfg.contains("host")) {
        host = cfg["host"];
    }

    unsigned int port = 0;
    if (cfg.contains("port")) {
        port = cfg["port"];
    }

    std::string login = "optirepair";
    if (cfg.contains("login")) {
        login = cfg["login"];
    }

    std::string password;
    if (cfg.contains("password")) {
        password = cfg["password"];
    }

    // инициализация библиотеки
    mysql_init(&m_mysqlDriver);

    uv_timer_init(uv_default_loop(), &m_callbackTimer);
    m_callbackTimer.data = nullptr;

    m_connection = mysql_real_connect(&m_mysqlDriver, host.c_str(), login.c_str(), password.c_str(), cluster.c_str(), port,
                                      nullptr, 0);
    if (m_connection == nullptr) {
        throw std::runtime_error(std::string("MySQL connection error: ") + mysql_error(&m_mysqlDriver));
    }

    mysql_select_db(m_connection, cluster.c_str());
}

void MySQLDB::updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    char newStatus[1024];
    mysql_real_escape_string(m_connection, newStatus, status.c_str(), status.length());

    std::string query = std::string("update configs set status = '") + newStatus + "' where serviceID='" + serviceId + "'";
    auto query_state = mysql_query(m_connection, query.c_str());
    if (query_state && m_callback && !stopPropagation) {
        m_callback->databaseGotError(this, mysql_error(m_connection));
        return;
    }

    if (mysql_affected_rows(m_connection) != 1) {
        ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Статус не изменен");
    }
}

ModuleDescription MySQLDB::getServiceName(const std::string &serviceID) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string("NOT IMPLEMENTED"));
}

void MySQLDB::storeError(const std::string &data) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string("NOT IMPLEMENTED"));

}

void MySQLDB::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    std::string query = "SELECT config FROM " + moduleType + " WHERE guid='" + serviceId + "'";
    auto query_state = mysql_query(m_connection, query.c_str());
    if (query_state) {
        throw std::runtime_error(std::string("MySQL error: ") + mysql_error(m_connection) );
    }

    if (m_callbackTimer.data == nullptr) {
        auto data = new callback_data;
        data->m_callback = m_callback;
        data->m_connection = m_connection;
        data->m_block = block;
        data->query = query;
        data->m_guid = serviceId;
        m_callbackTimer.data = data;
        uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getConfig), 0, 0);
    }
}

std::string MySQLDB::storeData(long long int timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    std::string tableName = senderId + "_" + type;

    std::string query = "INSERT INTO " + tableName + " (timestamp";

    if (useLevel)
        query += ", level";

    auto itemList = data->getItems();

    for (const auto& field : itemList) {
        if (field == "timestamp" || (useLevel && field == "level")) {
            continue;
        }
        if (field == "values")
            query += ", `" + field + "`";
        else
            query += ", " + field;
    }

    query += ") VALUES (" + std::to_string(timestamp) + (useLevel ?  ", " + std::to_string(level) : "");

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, query);

    for (const auto& item : itemList) {
        if (item == "timestamp" || (useLevel && item == "level")) {
            continue;
        }
        auto itemType = data->getItemType(item);

        std::string value;
        char *buffer;

        switch (itemType) {
            case OR_TYPE_INT:
                query += ", " + data->getString(item);
                break;
            case OR_TYPE_FLOAT:
                query += ", " + data->getString(item);
                break;
            case OR_TYPE_DOUBLE:
                query += ", " + data->getString(item);
                break;
            case OR_TYPE_LONG:
                query += ", " + data->getString(item);
                break;
            case OR_TYPE_STRING:
                value = data->getString(item);
                buffer = static_cast<char *>(malloc(value.length() * 2 + 1));
                mysql_real_escape_string(m_connection, buffer, value.c_str(), value.length());
                query += ", '" + std::string(buffer) + "'";
                free(buffer);
                break;
            case OR_TYPE_ARRAY:
                value = data->getString(item);
                buffer = static_cast<char *>(malloc(value.length() * 2 + 1));
                mysql_real_escape_string(m_connection, buffer, value.c_str(), value.length());
                query += ", '" + std::string(buffer) + "'";
                free(buffer);
                break;
            default:
                throw std::runtime_error("storeData(): type " + std::to_string(itemType) + " not implemented!!!");
        }
    }

    query += ")";

    auto query_state = mysql_query(m_connection, query.c_str());
    if (query_state) {
        throw std::runtime_error(std::string("MySQL error: ") + mysql_error(m_connection) );
    }

    if (mysql_affected_rows(m_connection) != 1) {
        ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Статус не изменен");
    }

    return std::to_string(mysql_insert_id(m_connection));
}

void MySQLDB::timer_callback_getConfig(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<callback_data *>(timer->data);

    auto res = mysql_store_result(data->m_connection);
    timer->data = nullptr;

    if (!res) {
        ordebug::warning(__PRETTY_FUNCTION__, __LINE__, "Конфиг не найден, ошибка: " + std::string(mysql_error(data->m_connection)));
        data->m_callback->databaseGotConfig(data->m_guid, data->m_block);
    } else {
        auto row = mysql_fetch_row(res);
        if (!row) {
            ordebug::warning(__PRETTY_FUNCTION__, __LINE__, "Нет записей с конфигом");
            data->m_callback->databaseGotConfig(data->m_guid, data->m_block);
        } else {
            //TODO: парсинг конфига
            data->m_callback->databaseGotConfig(data->m_guid, data->m_block);
        }
        mysql_free_result(res);
    }
    delete data;
}

void MySQLDB::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void MySQLDB::updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void MySQLDB::createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void MySQLDB::getConfigsByType(const std::string &dataType, const DataBlock &block) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void MySQLDB::requestData(DataQuery pQuery) {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

std::vector<std::string> MySQLDB::getServiceList() {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

int MySQLDB::removeData(std::string serviceId, std::string messageType, long long int timestamp) {
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void MySQLDB::checkStatus() {
    if (m_connection == nullptr) {
        throw std::runtime_error("Can't execute operation: Client not connected to MySQL server");
    }
    if(mysql_ping(m_connection) != 0) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, mysql_error(m_connection));
        if (m_callback) {
            m_callback->databaseGotError(this, "Connection error");
        }
    }
}
