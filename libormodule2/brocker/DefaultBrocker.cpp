//
// Created by kulakov on 14.04.2021.
//

#include "DefaultBrocker.h"

#include "../ordebug.h"

using namespace optirepair;

DefaultBrocker::~DefaultBrocker() {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Delete brocker");
}

void DefaultBrocker::init(const std::string& clusterName, const std::string &serviceId, const std::string &config, IBrockerCallback *callback) {
    std::ignore = callback;
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "(cluster="+clusterName + ", serviceId=" + serviceId + ", config=" + config + ", ptr)");
}

void DefaultBrocker::sendNotification(long long int timestamp, int level, const std::string &eventId, const std::string &sender,
                                      const std::string &type, DataBlock *data) {
    std::ignore = data;
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "timestamp=" + std::to_string(timestamp) + "; level=" +
            std::to_string(level) + "; eventId=" + eventId + "; sender=" + sender + "; type=" + type);
}

void DefaultBrocker::sendErrorEvent(long long int timestamp, int errorCode, const std::string &message, bool stopPropagation) {
    ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "timestamp=" + std::to_string(timestamp) + "; errorCode=" +
            std::to_string(errorCode) + "; message=" + message + "; stopPropagation=" +
            std::to_string(stopPropagation));
}

void DefaultBrocker::sendActionResponse(long long timestamp, const std::string &type, const std::string &sender,
                                        DataBlock *data) {
    std::ignore = data;
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "timestamp=" + std::to_string(timestamp) +
            "; sender=" + sender + "; type=" + type);
}

void DefaultBrocker::sendActionRequest(long long int timestamp, const std::string &serviceId, const std::string &type,
                                const std::string &sender, DataBlock *data) {
    std::ignore = data;
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "timestamp=" + std::to_string(timestamp) +
            "; serviceId=" + serviceId +
            "; sender=" + sender + "; type=" + type);
}

void DefaultBrocker::subscribe(const std::string &serviceId) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "serviceId=" + serviceId);
}

void DefaultBrocker::unsubscribe(const std::string &serviceId) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "serviceId=" + serviceId);
}

void DefaultBrocker::sendConfigNotification(const std::string &sender, const std::string &data, bool stopPropagation) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__ , "sender=" + sender + "; data=" + data + "stopPropagation=" +
            std::to_string(stopPropagation));
}

void DefaultBrocker::sub_controller(){
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "");
}
