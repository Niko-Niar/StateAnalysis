//
// Created by kulakov on 14.04.2021.
//

#include "DefaultDB.h"

#include "../ordebug.h"

using namespace optirepair;

DefaultDB::~DefaultDB() {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Call DefaultDB::~DefaultDB()");
}

void DefaultDB::init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) {
    std::ignore = callback;
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "cluster=" + cluster + "; moduleType=" + moduleType + "serviceId=" + serviceId + "; config=" +
                                                  config + ", ...)");

}

ModuleDescription DefaultDB::getServiceName(const std::string &serviceID) {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Call DefaultDB::getServiceName(" + serviceID + ")");
    ModuleDescription ret;
    ret.name = "bebebe";
    ret.type = "lalala";
    ret.description = "kakaka";
    return ret;
}

void DefaultDB::storeError(const std::string &data) {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Call DefaultDB::storeError(" + data + ")");
}

void DefaultDB::updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "moduleType=" + moduleType + "; serviceId=" + serviceId +
            "; status=" + status + "; stopPropagation=" + (stopPropagation ? "true" : "false") + ")");
}

std::string DefaultDB::storeData(long long timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type,  DataBlock *block) {
    std::ignore = block;
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Call DefaultDB::storeData(" + std::to_string(timestamp) + ", " + (useLevel ? "true, " : "false, ") +
        std::to_string(level) + ", " + senderType + ", " + senderId + ", " + type + ", ...)");
    return {};
}

void DefaultDB::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    std::ignore = block;
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "moduleType=" + moduleType + "; serviceId=" + serviceId);
}

void DefaultDB::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "moduleType=" + moduleType + "; serviceId=" + serviceId + "; config=" + config + ")");
}

void DefaultDB::updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    std::ignore = block;
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "moduleType=" + moduleType + "; serviceId=" + serviceId);
}

void DefaultDB::createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) {
    std::ignore = block;
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "tableName" + tableName + "; isEventTable=" +
            std::to_string(isEventTable) + "; senderType=" + senderType + "; senderId=" + senderId);
}

void DefaultDB::getConfigsByType(const std::string &dataType, const DataBlock &block) {
    std::ignore = block;

    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Call DefaultDB::getConfigsByType(" + dataType + ", ...)");
}

void DefaultDB::requestData(DataQuery pQuery) {
    std::ignore = pQuery;

    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Call DefaultDB::requestData(...)");
}

std::vector<std::string> DefaultDB::getServiceList() {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "");
    return {};
}

int DefaultDB::removeData(std::string serviceId, std::string messageType, long long int timestamp) {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "serviceId=" + serviceId + "; messageType=" + messageType + "; timestamp=" +
            std::to_string(timestamp));
    return 0;
}

void DefaultDB::checkStatus() {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "");
}
