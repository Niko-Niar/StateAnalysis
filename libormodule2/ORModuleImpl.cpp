//
// Created by kulakov on 13.04.2021.
//

#include "ORModuleImpl.h"

#include "orfile.h"
#include "ordebug.h"
#include "ORModule.h"

#include "brocker/DefaultBrocker.h"
#include "database/DefaultDB.h"

#include <stdexcept>
#include <climits>
#include <nlohmann/json.hpp>

using namespace optirepair;
using namespace nlohmann;

void ORModuleImpl::init(const std::string& moduleType, const std::string &service_id, ORModuleCallback *cb, const std::string &config_path) {
    m_serviceId = service_id;
    m_callback = cb;
    m_configPath = config_path;
    m_moduleType = moduleType;
    optirepair::ordebug::storeToFile(service_id, true); // включение записи предупреждающих и ошибочных данных в лог если это возможно

    // загрузка адаптеров из конфига
    loadAdapters();

    //TODO: добавить дефолтные обработчики брокера и БД
    addDefaults();

    //TODO: убрать инициализацию orfile
    orfile::init(config_path);

    if (m_callback != nullptr)
        m_callback->orOnReady();
}

void ORModuleImpl::loadAdapters() {
    std::ifstream configFile(m_configPath);
    m_clusterName = "optirepair";
    m_storageDir = OR_DEFAULT_STORAGE_PATH;
    try {
        json configJson;
        if (!configFile) {
            throw std::runtime_error("Failed to open" + m_configPath);
        }

        configFile >> configJson;


        if (configJson["storage"] != nullptr &&
            configJson["storage"]["filesystem"] != nullptr &&
            configJson["storage"]["filesystem"]["path"] != nullptr) {
            m_storageDir = configJson["storage"]["filesystem"]["path"];
        }

        // название кластера требуется до подключения к БД
        if (configJson.contains("name")) {
            m_clusterName = configJson["name"];
        }

        if (!configJson.contains("connections")) {
            throw std::runtime_error("No connections section in config file");
        }

        if (configJson["connections"].contains("brocker")) {
            auto brockerJson = configJson["connections"]["brocker"];
            std::string type = brockerJson["type"];
            m_brocker = IBrocker::getBrocker(type);
            std::string config = brockerJson["config"].dump();
            m_brocker->init(m_clusterName, m_serviceId, config, this);
        }

        if (configJson["connections"].contains("configdb")) {
            auto dbJson = configJson["connections"]["configdb"];
            std::string type = dbJson["type"];
            m_configDB = IDatabase::getDatabase(type);
            std::string config = dbJson["config"].dump();
            m_configDB->init(m_clusterName, m_moduleType, m_serviceId, config, this);
        }

        if (configJson["connections"].contains("sourcedb")) {
            auto dbJson = configJson["connections"]["sourcedb"];
            std::string type = dbJson["type"];
            m_sourceDB = IDatabase::getDatabase(type);
            std::string config = dbJson["config"].dump();
            m_sourceDB->init(m_clusterName, m_moduleType, m_serviceId, config, this);
        }

        if (configJson["connections"].contains("eventdb")) {
            auto dbJson = configJson["connections"]["eventdb"];
            std::string type = dbJson["type"];
            m_eventDB = IDatabase::getDatabase(type);
            std::string config = dbJson["config"].dump();
            m_eventDB->init(m_clusterName, m_moduleType, m_serviceId, config, this);
        }
    } catch (const std::exception& ex) {
        ordebug::error(__PRETTY_FUNCTION__, __LINE__, ex.what());
    }

    configFile.close();
}

void ORModuleImpl::addDefaults() {
    // проверка брокеров
    if (m_brocker == nullptr) {
        m_brocker = new DefaultBrocker();
        m_brocker->init(m_clusterName, m_serviceId, "", this);
    }

    if (m_configDB == nullptr) {
        m_configDB = new DefaultDB();
        m_configDB->init(m_clusterName, m_moduleType, m_serviceId, "", this);
    }

    if (m_sourceDB == nullptr) {
        m_sourceDB = new DefaultDB();
        m_sourceDB->init(m_clusterName, m_moduleType, m_serviceId, "", this);
    }

    if (m_eventDB == nullptr) {
        m_eventDB = new DefaultDB();
        m_eventDB->init(m_clusterName, m_moduleType, m_serviceId, "", this);
    }

    if (m_clusterName.empty()) {
        m_clusterName = "optirepair";
    }
    if (m_storageDir.empty()) {
        m_storageDir = OR_DEFAULT_STORAGE_PATH;
    }
}

std::string ORModuleImpl::getSettingsPath() const {
    char actualpath [PATH_MAX];
    auto ptr = realpath(m_configPath.c_str(), actualpath);
    if (ptr) {
        size_t pos = std::string(actualpath).find_last_of("\\/");
        if (std::string::npos == pos) {
            return (actualpath);
        } else {
            return std::string(actualpath).substr(0, pos);
        }
    }
    return "/etc/optirepair";
}

ORModuleImpl::~ORModuleImpl() {
    // удаление брокеров
    delete m_brocker;
    m_brocker = nullptr;

    // удаление адаптеров БД
    delete m_sourceDB;

    if (m_eventDB != m_sourceDB) {
        delete m_eventDB;
    }
    if (m_configDB != m_sourceDB && m_configDB != m_eventDB) {
        delete m_configDB;
    }
    m_sourceDB = nullptr;
    m_eventDB = nullptr;
    m_configDB = nullptr;
}

void ORModuleImpl::brockerGotConfigMessage() {
    if (m_configDB && m_configBlock) {
        m_configDB->getConfig(m_moduleType, m_serviceId, m_configBlock);
    } else {
        m_callback->orGotError(37, "Got config message, but config DB or Config instance not defined");
    }
}

void ORModuleImpl::brockerGotAction(const string &topic, const string &payload) {
    if (m_callback != nullptr) {
        m_callback->orGotAction(payload);
    }
}

void ORModuleImpl::brockerGotEvent(const string &topic, const string &payload) {
    if (m_callback != nullptr) {
        m_callback->orGotEvent(topic, payload);
    }
}

void ORModuleImpl::databaseGotConfig(const std::string &guid, DataBlock *config) {
    m_configBlock = config;
    if (m_callback) {
        m_callback->orGotConfig(guid, config);
    } else {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "No callback was set up");
    }
}

void ORModuleImpl::databaseGotOtherConfigs(const std::string &datatype, const std::map<std::string, DataBlock *>& vector) {
    if (m_callback) {
        m_callback->orGotOtherConfigs(datatype, vector);
    } else {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "No callback was set up");
    }
}

void ORModuleImpl::databaseGotError(IDatabase *instance, const string &errorMessage) {
    ordebug::error(__PRETTY_FUNCTION__ , __LINE__, errorMessage);
    // TODO: более гибкий разбор сообщений об ошибках
}

void ORModuleImpl::databaseGotData(DataQuery query, DataBlock *result) {
    if (m_callback)
        m_callback->orGotStorageData(result->getBlockTimestamp(), query, result);
}
