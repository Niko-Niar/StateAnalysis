//
// Created by kulakov on 06.04.2021.
//

#include "DBMongo.h"
#include "../ordebug.h"
#include "FileDB.h"

#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/client.hpp>
#include <nlohmann/json.hpp>

using namespace optirepair;

using namespace nlohmann;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void DBMongo::updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    std::ignore = moduleType;
    std::ignore = stopPropagation;

    mongocxx::pool pool{mongocxx::uri{m_mongoDbUri}};
    auto client = pool.acquire();
    auto db = (*client).database(m_database);

    auto searchDoc = make_document(kvp("_id", bsoncxx::oid(serviceId)));
    auto updateDoc = make_document(kvp("$set", make_document(kvp("status", status))));

    db.collection("configs").update_one(searchDoc.view(), updateDoc.view());

}

void DBMongo::init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) {
    std::ignore = moduleType;
    std::ignore = serviceId;

    m_database = cluster;
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
    m_mongoDbUri = "mongodb://" + host + ":" + std::to_string(port);

    uv_timer_init(uv_default_loop(), &m_callbackTimer);
    m_callbackTimer.data = nullptr;
}

ModuleDescription DBMongo::getServiceName(const std::string &serviceID) {
    mongocxx::pool pool{mongocxx::uri{m_mongoDbUri}};
    auto client = pool.acquire();
    auto db = (*client).database(m_database);

    auto searchDoc = make_document(kvp("_id", bsoncxx::oid(serviceID)));
    auto configDoc = db.collection("configs").find_one(searchDoc.view());

    if (!configDoc) {
        return {};
    }
    ModuleDescription ret;
    ret.name = configDoc->view()["name"].get_utf8().value.to_string();
    ret.description = configDoc->view()["description"].get_utf8().value.to_string();
    auto serviceTypeId = configDoc->view()["service_type"].get_oid();
    auto searchServiceDoc = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", serviceTypeId));
    auto serviceTypeDoc = db.collection("servicetypes").find_one(searchServiceDoc.view()); // db.servicetypes.find("_id":ObjectId("5e8c5ba93c13e359434f0482"));
    ret.type = serviceTypeDoc->view()["name"].get_utf8().value.to_string(); //service_type -> servicetypes.ObjectId() -> name

    return ret;
}

std::string DBMongo::storeData(long long int timestamp, bool useLevel, int level,
                               const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data) {
    std::ignore = senderType;

    json event;
    event["timestamp"] = timestamp;
    event["type"] = type;
    if (useLevel) {
        event["level"] = level;
    }
    event["sender"] = senderId;
    event["data"] = nlohmann::json::parse(data->toJson());

    mongocxx::pool pool{mongocxx::uri{m_mongoDbUri}};
    auto client = pool.acquire();
    auto db = (*client).database(m_database);

    bsoncxx::document::value eventData = bsoncxx::from_json(event.dump());
    auto result = db.collection(senderId).insert_one(eventData.view());

    return result->inserted_id().get_oid().value.to_string();
}

void DBMongo::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    std::ignore = moduleType;

    mongocxx::pool pool{mongocxx::uri{m_mongoDbUri}};
    auto client = pool.acquire();
    auto db = (*client).database(m_database);

    auto searchDoc = make_document(kvp("_id", bsoncxx::oid(serviceId)));
    auto configDoc = db.collection("configs").find_one(searchDoc.view());
    if (!configDoc) {
        ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Нет коллекции с oid: " + serviceId + " в MongoDB (db: " + m_database + "; uri: " + m_mongoDbUri + ")");
        return;
    }
    std::string config = bsoncxx::to_json(configDoc->view());

    if (!block->parseFromJson(config)) {
        return;
    }

    // отправка конфига
    if (m_callbackTimer.data == nullptr) {
        auto data = new file_callback_data(m_callback);
        data->setBlock(block);
        data->setGuid(serviceId);
        m_callbackTimer.data = data;
        uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getConfig), 0, 0);
    }

}

void DBMongo::storeError(const std::string &data) {
    std::ignore = data;

    throw std::runtime_error("DBMongo::storeError() NOT IMPLEMENTED!!!");

}

void DBMongo::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    std::ignore = moduleType;

    mongocxx::pool pool{mongocxx::uri{m_mongoDbUri}};
    auto client = pool.acquire();
    auto db = (*client).database(m_database);

    bsoncxx::document::value eventData = bsoncxx::from_json(config);
    auto searchDoc = make_document(kvp("_id", bsoncxx::oid(serviceId)));
    auto updateDoc = make_document(kvp("$set", make_document(kvp("settings", eventData))));

    db.collection("configs").update_one(searchDoc.view(), updateDoc.view());
}

void DBMongo::updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    std::ignore = moduleType;

    mongocxx::pool pool{mongocxx::uri{m_mongoDbUri}};
    auto client = pool.acquire();
    auto db = (*client).database(m_database);

    auto data = block->getFilledItems();
    ordebug::info(__PRETTY_FUNCTION__, __LINE__, data);

    bsoncxx::document::value eventData = make_document(kvp("$set", bsoncxx::from_json(data)));
    auto searchDoc = make_document(kvp("_id", bsoncxx::oid(serviceId)));

    db.collection("configs").update_one(searchDoc.view(), eventData.view());
}

void DBMongo::timer_callback_getConfig(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<file_callback_data *>(timer->data);

    timer->data = nullptr;

    data->processBlock();
    delete data;
}

void DBMongo::createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) {
    std::ignore = tableName;
    std::ignore = isEventTable;
    std::ignore = senderType;
    std::ignore = senderId;
    std::ignore = block;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void DBMongo::getConfigsByType(const std::string &dataType, const DataBlock &block) {
    //throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));

    std::map<std::string, DataBlock *> ret;

    mongocxx::pool pool{mongocxx::uri{m_mongoDbUri}};
    auto client = pool.acquire();
    auto db = (*client).database(m_database);

    auto searchDoc = make_document(kvp("name", dataType));
    auto configDoc = db.collection("servicetypes").find_one(searchDoc.view());
    if (!configDoc) {
        ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Нет коллекции с name: " + dataType + " в MongoDB (db: " + m_database + "; uri: " + m_mongoDbUri + ")");
        return;
    }
    std::string service_type = bsoncxx::to_json(configDoc->view());
    //ordebug::info(__PRETTY_FUNCTION__, __LINE__, service_type);
    nlohmann::json cfg = nlohmann::json::parse(service_type);
    std::string type_oid = cfg["_id"]["$oid"];
    std::string guid;
    //ordebug::info(__PRETTY_FUNCTION__, __LINE__, type_oid);

    searchDoc = make_document(kvp("service_type", bsoncxx::oid(type_oid)));
    auto configDocs = db.collection("configs").find(searchDoc.view());
    for (auto doc : configDocs) {
        cfg = nlohmann::json::parse(bsoncxx::to_json(doc));
        guid = cfg["_id"]["$oid"];

        auto bblock = block.createInstance();

        if (!bblock->parseFromJson(cfg.dump())) {
            continue;
        }
        ret[guid] = bblock;
    }


    // отправка конфига
    if (m_callbackTimer.data == nullptr) {
        auto data = new file_callback_data(m_callback);
        data->append(dataType, ret);
        m_callbackTimer.data = data;
        uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getConfigsByType), 0, 0);
    } else {
        file_callback_data *data = static_cast<file_callback_data *>(m_callbackTimer.data);
        data->append(dataType, ret);
    }
}

void DBMongo::requestData(DataQuery pQuery) {
    std::ignore = pQuery;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

std::vector<std::string> DBMongo::getServiceList() {
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

int DBMongo::removeData(std::string serviceId, std::string messageType, long long int timestamp) {
    std::ignore = serviceId;
    std::ignore = messageType;
    std::ignore = timestamp;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void DBMongo::timer_callback_getConfigsByType(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<file_callback_data *>(timer->data);

    timer->data = nullptr;

    data->processList();
    delete data;
}

void DBMongo::checkStatus() {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "NOT IMPLEMENTED");
}

