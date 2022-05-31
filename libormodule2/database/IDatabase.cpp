//
// Created by kulakov on 19.05.2021.
//

#include <stdexcept>
#include <nlohmann/json.hpp>
#include "IDatabase.h"

#include "../ordebug.h"

#ifdef _MYSQL_FOUND_
#include "MySQLDB.h"
#endif

#include "FileDB.h"

#ifdef _SQLITE_FOUND_
#include "SqliteDB.h"
#endif

#ifdef _CLICKHOUSE_FOUND_
#include "ClickhouseDB.h"
#endif

#ifdef _CURL_FOUND_
#include "SystemAPIDB.h"
#endif

#ifdef _MONGODB_FOUND_
#include "DBMongo.h"
#endif

IDatabase *IDatabase::getDatabase(const std::string &type) {
    if (type == "file") {
        return new FileDB();
#ifdef _MYSQL_FOUND_
    } else if (type == "mysql") {
        return new MySQLDB();
#endif
#ifdef _SQLITE_FOUND_
    } else if (type == "sqlite") {
        return new SqliteDB();
#endif
#ifdef _MONGODB_FOUND_
    } else if (type == "mongodb") {
        return new DBMongo();
#endif
#ifdef _CLICKHOUSE_FOUND_
    } else if (type == "clickhouse") {
        return new ClickhouseDB();
#endif
#ifdef _CURL_FOUND_
    } else if (type == "systemapi") {
        return new SystemAPIDB();
#endif
    } else {
        throw std::runtime_error("Unknown configdb type " + type);
    }
}
