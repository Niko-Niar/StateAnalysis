//
// Created by kulakov on 13.04.2021.
//

#include "ORModule.h"

#include <utility>
#include "ORModuleImpl.h"

#include "ordebug.h"
#include "orutils.h"
#include "SimpleDataBlock.h"

using namespace optirepair;

#define UNUSED(x) (void)(x)

ORModuleImpl * ORModule::m_instance = nullptr;
uv_timer_t *ORModule::timer_req = nullptr;

void ORModule::sendErrorEvent(int errorCode, const std::string &errorMessage, bool stopPropagation) {
    auto timestamp = optirepair::orutils::timeInMilliseconds();
    getInstance()->brocker()->sendErrorEvent(timestamp, errorCode, errorMessage, stopPropagation);
}

void ORModule::updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    getInstance()->configDB()->updateStatus(moduleType, serviceId, status, stopPropagation);
    getInstance()->brocker()->sendConfigNotification(serviceId, R"({"status": ")" + status + "\"}", stopPropagation);
}

ORModuleImpl *ORModule::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new ORModuleImpl();

        if (timer_req == nullptr) {
            timer_req = new uv_timer_t();
            uv_timer_init(uv_default_loop(), timer_req);
            uv_timer_start(ORModule::timer_req, ORModule::r_onUpdate, 0, 1000 * OR_DEFAULT_TIMEOUT);
        }
    }

    return m_instance;
}

std::string ORModule::getServiceId() {
    return getInstance()->serviceId();
}

void
ORModule::sendActionResponse(long long int timestamp, const std::string &type, const std::string &sender, DataBlock *data) {
    getInstance()->brocker()->sendActionResponse(timestamp, type, sender, data);
}

void ORModule::sendActionRequest(long long int timestamp, const std::string &serviceId, const std::string &type,
                          const std::string &sender, DataBlock *data) {
    getInstance()->brocker()->sendActionRequest(timestamp, serviceId, type, sender, data);
}

void ORModule::init(const std::string& moduleType, const std::string &service_id, ORModuleCallback *cb) {
    init(moduleType, service_id, cb, OR_DEFAULT_CONFIG_PATH);
}

void ORModule::init(const std::string& moduleType, const std::string &service_id, ORModuleCallback *cb, const std::string &config_path) {
    getInstance()->init(moduleType, service_id, cb, config_path);
}

void ORModule::storeSourceData(long long int timestamp, const std::string &senderType, const std::string &senderId, const std::string &type,
                               DataBlock *data, const std::vector<std::string>& exclude_fields) {
    auto id = getInstance()->sourceDb()->storeData(timestamp, false, 0, senderType, senderId, type, data);
    for (auto& field : exclude_fields) {
        data->pop(field);
    }
    getInstance()->brocker()->sendNotification(timestamp, 0, id, senderId, type, data);
}

void ORModule::sendNotification(long long int timestamp, int level, const std::string &sender,
                                const std::string &type, DataBlock *data) {
    getInstance()->brocker()->sendNotification(timestamp, level, std::string(), sender, type, data);
}

void
ORModule::storeEvent(long long int timestamp, int level, const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data) {
    auto id = getInstance()->eventDb()->storeData(timestamp, true, level, senderType, senderId, type, data);
    getInstance()->brocker()->sendNotification(timestamp, level, id, senderId, type, data);
}

void ORModule::cleanup() {
    if (m_instance != nullptr) {
        delete m_instance;
        m_instance = nullptr;
    }
    if (timer_req != nullptr) {
        uv_timer_stop(timer_req);
        uv_close(reinterpret_cast<uv_handle_t *> (timer_req), ORModule::on_timer_close_complete);
        timer_req = nullptr;
    }
}

std::string ORModule::getClusterName() {
    return getInstance()->getClusterName();
}

std::string ORModule::getStorageDir() {
    return getInstance()->getStorageDir();
}

std::string ORModule::getSettingsPath() {
    return getInstance()->getSettingsPath();
}

void ORModule::on_timer_close_complete(uv_handle_t *handle) {
    free(handle);
    timer_req = nullptr;
}

void ORModule::r_onUpdate(uv_timer_s* timer) {
    UNUSED(timer);
        
    getInstance()->brocker()->sub_controller();

    if(getInstance()->sourceDb())
        getInstance()->sourceDb()->checkStatus();

    if(getInstance()->eventDb())
        getInstance()->eventDb()->checkStatus();

    if(getInstance()->configDB())
        getInstance()->configDB()->checkStatus();

    if (getInstance()->callback() != nullptr)
        getInstance()->callback()->orOnUpdate();
}

void ORModule::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    if (getInstance()->configDB()) {
        getInstance()->configDB()->getConfig(moduleType, serviceId, block);
    } else {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Config DB not initialised");
    }
}

void ORModule::subscribeToService(const std::string& serviceId) {
    getInstance()->brocker()->subscribe(serviceId);
}

void ORModule::unsubscribeToService(const std::string &serviceId) {
    getInstance()->brocker()->unsubscribe(serviceId);
}

void ORModule::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    getInstance()->configDB()->sendConfig(moduleType, serviceId, config);
    getInstance()->brocker()->sendConfigNotification(serviceId, config, false);
}

ModuleDescription ORModule::getServiceName(const std::string &serviceId) {
    return getInstance()->configDB()->getServiceName(serviceId);
}

void ORModule::addConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    if (getInstance()->configDB()) {
        getInstance()->configDB()->updateConfigField(moduleType, serviceId, block);
    } else {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Config DB not initialised");
    }
}

void ORModule::getConfigsByType(const char *type, const DataBlock &pBlock) {
    if (getInstance()->configDB()) {
        getInstance()->configDB()->getConfigsByType(type, pBlock);
    } else {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Config DB not initialised");
    }
}

void ORModule::close() {
    ORModule::updateStatus(ORModule::getModuleType(), ORModule::getServiceId(), "Выключен", false);
    delete m_instance;
    m_instance = nullptr;
}

std::vector<std::string> ORModule::getServiceList() {
    if (getInstance()->configDB())
        return getInstance()->configDB()->getServiceList();
    return {};
}

std::string ORModule::getModuleType() {
    return m_instance->getModuleType();
}

void ORModule::requestData(DataQuery query) {
    if (getInstance()->sourceDb()) {
        getInstance()->sourceDb()->requestData(std::move(query));
    } else {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Config DB not initialised");
    }
}

int ORModule::removeFileData(const std::string& serviceId, const std::string& messageType, long long int timestamp) {
    std::ignore = serviceId;
    std::ignore = messageType;
    std::ignore = timestamp;
    ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "NOT IMPLEMENTED!!!");
    return 0;
}

int ORModule::removeSourceData(const std::string& serviceId, const std::string& messageType, long long int timestamp) {
    return getInstance()->sourceDb()->removeData(serviceId, messageType, timestamp);
}

int ORModule::removeEventData(const std::string& serviceId, const std::string& messageType, long long int timestamp) {
    return getInstance()->eventDb()->removeData(serviceId, messageType, timestamp);
}
