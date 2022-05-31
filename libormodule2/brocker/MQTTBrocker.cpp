//
// Created by kulakov on 25.03.2021.
//

#include "MQTTBrocker.h"

#include "../ordebug.h"

#include <cstring>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <time.h>

#define UNUSED(x) (void)(x)
#define STR_FORMAT_PING "{\"data\":{\"check_str\":\"connection_check\"},\"timestamp\":%ld,\"type\":\"%s\",\"sender\":\"%s\"}"

#define MAX_PING_STR_LEN 500
#define NUM_CHECKED_PING_STR_SYMBOLS 41

const char * MQTT_SERVICE_CONFIGS = "service/configs/";
const char * MQTT_CONFIGS = "service/configs/";
const char * MQTT_SERVICE_EVENTS = "service/events/";
const char * MQTT_SERVICE_ERRORS = "service/errors/";
const char * MQTT_SERVICE_ACTIONS = "service/actions/";

using namespace nlohmann;

void MQTTBrocker::init(const std::string& clusterName, const std::string& serviceId, const std::string& config, IBrockerCallback *callback) {
    std::ignore = clusterName;

    m_serviceId = serviceId;
    m_callback = callback;

    // rabbitmq
    nlohmann::json rabbitJson = nlohmann::json::parse(config);
    std::string mqttHost = "localhost";
    if (rabbitJson.contains("host"))
        mqttHost = rabbitJson["host"];
    int mqttPort = 1883;
    if (rabbitJson.contains("port"))
        mqttPort = rabbitJson["port"];
    std::string mqttLogin;
    if (rabbitJson.contains("login"))
        mqttLogin = rabbitJson["login"];
    std::string mqttPassword;
    if (rabbitJson.contains("password"))
        mqttPassword = rabbitJson["password"];


    if (m_mqttClient != nullptr) {
        mosquitto_reinitialise(m_mqttClient, m_serviceId.c_str(), true, this);
    } else {
        m_mqttClient = mosquitto_new(m_serviceId.c_str(), true, this);
    }

    // подключение обработчиков
    mosquitto_connect_callback_set(m_mqttClient, MQTTBrocker::mqtt_on_connect);
    mosquitto_disconnect_callback_set(m_mqttClient, MQTTBrocker::mqtt_on_disconnect);
    mosquitto_publish_callback_set(m_mqttClient, MQTTBrocker::mqtt_on_publish);
    mosquitto_message_callback_set(m_mqttClient, MQTTBrocker::mqtt_on_message);
    mosquitto_log_callback_set(m_mqttClient, MQTTBrocker::on_log);

    if (!mqttLogin.empty()) {
        mosquitto_username_pw_set(m_mqttClient, mqttLogin.c_str(), mqttPassword.c_str());
    }
    mosquitto_connect_async(m_mqttClient, mqttHost.c_str(), mqttPort, 60);
    mosquitto_loop_start(m_mqttClient);

    m_Sub_TopicAndCheck[std::string(MQTT_SERVICE_CONFIGS) + m_serviceId] = SUCCESSFUL_CHECK;
    mosquitto_subscribe(m_mqttClient, nullptr, (std::string(MQTT_SERVICE_CONFIGS) + m_serviceId).c_str(), 0);
    
    m_Sub_TopicAndCheck[std::string(MQTT_SERVICE_ACTIONS) + m_serviceId] = SUCCESSFUL_CHECK;
    mosquitto_subscribe(m_mqttClient, nullptr, (std::string(MQTT_SERVICE_ACTIONS) + m_serviceId).c_str(), 0);
}

void MQTTBrocker::mqtt_on_connect(struct mosquitto * instance, void * object, int status) {
    UNUSED(instance);
    UNUSED(object);
    optirepair::ordebug::info(__PRETTY_FUNCTION__, __LINE__, std::string("status=") + std::to_string(status));
}

void MQTTBrocker::mqtt_on_disconnect(struct mosquitto * instance, void * object, int status) {
    UNUSED(instance);
    UNUSED(object);
    optirepair::ordebug::info(__PRETTY_FUNCTION__, __LINE__, std::string("status=") + std::to_string(status) + "; desc=" + mosquitto_strerror(status));
}

void MQTTBrocker::mqtt_on_publish(struct mosquitto *instance, void *obj, int status) {
    UNUSED(instance);
    UNUSED(obj);
    optirepair::ordebug::info(__PRETTY_FUNCTION__, __LINE__, std::string("status=") + std::to_string(status));
}

void MQTTBrocker::mqtt_on_message(struct mosquitto *instance, void *obj, const struct mosquitto_message *message) {
    UNUSED(instance);

    auto *that = static_cast<MQTTBrocker *>(obj);
    optirepair::ordebug::info(__PRETTY_FUNCTION__, __LINE__, message->topic);

    if(that->m_Sub_TopicAndCheck.find(std::string(message->topic)) != that->m_Sub_TopicAndCheck.end()){
        that->m_Sub_TopicAndCheck[std::string(message->topic)] = SUCCESSFUL_CHECK;
        
        if((message->payloadlen > NUM_CHECKED_PING_STR_SYMBOLS) && (strncmp(STR_FORMAT_PING, static_cast<const char*>(message->payload),
                                                                        NUM_CHECKED_PING_STR_SYMBOLS) == 0)){
            return;
        }
    } else {
        optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Received a message on an unregistered topic!");
        return; 
    }
        
    if (message->topic == (MQTT_SERVICE_CONFIGS + that->m_serviceId)) {
        that->m_callback->brockerGotConfigMessage();
        //getConfig();
        return;
    }

    std::string payload;

    if (message->topic == (MQTT_SERVICE_ACTIONS + that->m_serviceId)) {
        payload.resize(static_cast<unsigned long>(message->payloadlen) + 1);
        std::memcpy(&payload[0], message->payload, static_cast<size_t>(message->payloadlen));
        payload[static_cast<unsigned long>(message->payloadlen)] = '\0';

        // проверка сообщения на валидность
        try {
            auto dataItem = nlohmann::json::parse(payload);
            if (!dataItem.contains("action")) {
                throw std::logic_error("Action does not contain field 'action'");
            }
            if (!dataItem.contains("data")) {
                throw std::logic_error("Action does not contain field 'data'");
            }
        } catch (const std::exception &ex) {
            optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, ex.what());
            return;
        }

        that->m_callback->brockerGotAction(message->topic, payload);
        //that->m_callback->orGotAction(payload);
        return;
    }

    for(auto it = that->m_Sub_TopicAndCheck.begin(); it != that->m_Sub_TopicAndCheck.end(); it++) {
        if((it->first).find(MQTT_SERVICE_EVENTS, 0) == 0){
            if(message->topic == it->first){
                payload.resize(static_cast<unsigned long>(message->payloadlen));
                std::memcpy(&payload[0], message->payload, static_cast<size_t>(message->payloadlen));

                try {
                    auto dataItem = nlohmann::json::parse(payload);
                    if (!dataItem.contains("sender")) {
                        throw std::logic_error("Action does not contain field 'sender'");
                    }
                    if (!dataItem.contains("type")) {
                        throw std::logic_error("Action does not contain field 'type'");
                    }
                    if (!dataItem.contains("level")) {
                        throw std::logic_error("Action does not contain field 'level'");
                    }
                    if (!dataItem.contains("timestamp")) {
                        throw std::logic_error("Action does not contain field 'timestamp'");
                    }
                    if (!dataItem.contains("data")) {
                        throw std::logic_error("Action does not contain field 'data'");
                    }
                } catch (const std::exception &ex) {
                    optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, ex.what());
                    return;
                }

                that->m_callback->brockerGotEvent(std::string((it->first).c_str() + strlen(MQTT_SERVICE_EVENTS)), payload);
            }
        }
    }
}

void MQTTBrocker::on_log(struct mosquitto *instance, void *obj, int level, const char *message) {
    UNUSED(instance);
    UNUSED(obj);

    optirepair::ordebug::info(__PRETTY_FUNCTION__ , __LINE__, std::string("Mosquitto (") + std::to_string(level) + "): " + message);
}

void MQTTBrocker::sendNotification(long long int timestamp, int level, const std::string &eventId, const std::string &sender,
                                   const std::string &type, DataBlock *data) {
    json message;
    message["timestamp"] = timestamp;
    message["type"] = type;
    message["level"] = level;
    if (!eventId.empty()) {
        message["event_id"] = eventId;
    }
    message["sender"] = sender;
    message["data"] = nlohmann::json::parse(data->toJson());
    std::string msg = message.dump();
    mosquitto_publish(m_mqttClient, nullptr, (MQTT_SERVICE_EVENTS + sender).c_str(), static_cast<int>(msg.size()), msg.c_str(), 0, false);
}

void MQTTBrocker::sendErrorEvent(long long int timestamp, int errorCode, const std::string& message, bool stopPropagation) {
    std::ignore = stopPropagation;
    json event;
    event["timestamp"] = timestamp;
    event["type"] = "error";
    event["sender"] = m_serviceId;
    event["data"]["code"] = errorCode;
    event["data"]["message"] = message;
    auto msg = event.dump();
    mosquitto_publish(m_mqttClient, nullptr, (MQTT_SERVICE_ERRORS + m_serviceId).c_str(), static_cast<int>(msg.size()), msg.c_str(),0,false);
}

void MQTTBrocker::sendActionResponse(long long int timestamp, const std::string &type, const std::string &sender, DataBlock *data) {
    json event;
    event["timestamp"] = timestamp;
    event["type"] = type;
    event["sender"] = sender;
    event["data"] = nlohmann::json::parse(data->toJson());
    auto message = event.dump();
    mosquitto_publish(m_mqttClient, nullptr, (MQTT_SERVICE_ACTIONS + type + "/" + sender).c_str(), static_cast<int>(message.size()), message.c_str(), 0, false);
}

void MQTTBrocker::sendActionRequest(long long int timestamp, const std::string& serviceId, const std::string &type, const std::string &sender, DataBlock *data) {
    json event;
    event["timestamp"] = timestamp;
    event["type"] = type;
    event["sender"] = sender;
    event["data"] = nlohmann::json::parse(data->toJson());
    auto message = event.dump();
    mosquitto_publish(m_mqttClient, nullptr, (MQTT_SERVICE_ACTIONS + serviceId).c_str(), static_cast<int>(message.size()), message.c_str(), 0, false);
}

void MQTTBrocker::sendConfigNotification(const std::string &sender, const std::string &data, bool stopPropagation) {
    std::ignore = stopPropagation;
    json event;
    event["_id"] = sender;
    event["data"] = nlohmann::json::parse(data);
    auto message = event.dump();
    mosquitto_publish(m_mqttClient, nullptr, MQTT_CONFIGS, static_cast<int>(message.size()), message.c_str(), 0, false);
}

void MQTTBrocker::subscribe(const std::string &serviceId) {
    if (serviceId.empty()) {
        optirepair::ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Try to subscribe to empty service");
        return;
    }

    if (m_Sub_TopicAndCheck.find(std::string(MQTT_SERVICE_EVENTS) + serviceId) != m_Sub_TopicAndCheck.end()) {
        optirepair::ordebug::warning(__PRETTY_FUNCTION__, __LINE__, "Service " + serviceId + " already subscribed");
    } else {
        mosquitto_subscribe(m_mqttClient, nullptr, (std::string(MQTT_SERVICE_EVENTS) + serviceId).c_str(), 0);
        m_Sub_TopicAndCheck[std::string(MQTT_SERVICE_EVENTS) + serviceId] = SUCCESSFUL_CHECK;
    }
}

void MQTTBrocker::unsubscribe(const std::string &serviceId) {

    if(serviceId.empty()) {
        optirepair::ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Try to unsubscribe empty service!");
        return;
    }

    std::map<std::string, CHECK_EXIST_SUB_RES>::iterator it;
    if((it = m_Sub_TopicAndCheck.find(std::string(MQTT_SERVICE_EVENTS) + serviceId)) != m_Sub_TopicAndCheck.end()){
        mosquitto_unsubscribe(m_mqttClient, nullptr, (it->first).c_str());
        m_Sub_TopicAndCheck.erase(it);
    } else {
        optirepair::ordebug::warning(__PRETTY_FUNCTION__, __LINE__, "Service " + serviceId + " not in list");
    }
}

void MQTTBrocker::sub_controller(){
    char payload[MAX_PING_STR_LEN];

    for(auto & it : m_Sub_TopicAndCheck) {
        switch(it.second){
            case SUCCESSFUL_CHECK :
                it.second = FAILED_CHECK_BY_DATA_OR_ALIEN_PING;
            break;

            case FAILED_CHECK_BY_DATA_OR_ALIEN_PING:
                it.second = FAILED_CHECK_BY_DATA_OR_ALIEN_PING_OR_OWN_PING;    
                sprintf(payload, STR_FORMAT_PING, time(nullptr)*1000, "new_check", m_serviceId.c_str());
                mosquitto_publish(m_mqttClient, nullptr, (it.first).c_str(), static_cast<int>(strlen(payload)), payload, 0, false);
            break;

            case FAILED_CHECK_BY_DATA_OR_ALIEN_PING_OR_OWN_PING:
                it.second = SUCCESSFUL_CHECK;
                mosquitto_subscribe(m_mqttClient, nullptr, (it.first).c_str(), 0);
                optirepair::ordebug::info(__PRETTY_FUNCTION__, __LINE__, std::string("Oversubscription: ") + it.first);
            break;

            default:
                optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Non-existent enumeration value!");
            return;
        }
    }
}

MQTTBrocker::~MQTTBrocker() {
    mosquitto_disconnect(m_mqttClient);
    mosquitto_loop_stop(m_mqttClient, false);
    mosquitto_lib_cleanup();
}

MQTTBrocker::MQTTBrocker() {
    mosquitto_lib_init();
}
