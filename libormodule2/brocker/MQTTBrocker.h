//
// Created by kulakov on 25.03.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_MQTTBROCKER_H
#define ROOT_OR_SERVER_MONITOR_MODULE_MQTTBROCKER_H

#include "IBrocker.h"

#include <string>
#include <mosquitto.h>
#include <map>

extern const char * MQTT_SERVICE_CONFIGS;
extern const char * MQTT_CONFIGS;
extern const char * MQTT_SERVICE_EVENTS;
extern const char * MQTT_SERVICE_ERRORS;
extern const char * MQTT_SERVICE_ACTIONS;

enum CHECK_EXIST_SUB_RES {SUCCESSFUL_CHECK, FAILED_CHECK_BY_DATA_OR_ALIEN_PING, FAILED_CHECK_BY_DATA_OR_ALIEN_PING_OR_OWN_PING};

class MQTTBrocker : public IBrocker {
public:
    // запрет копирования объекта
    MQTTBrocker(const MQTTBrocker& value) = delete;
    void operator=(const MQTTBrocker& value) = delete;

    MQTTBrocker();
    ~MQTTBrocker() override;

    void init(const std::string& clusterName, const std::string& serviceId, const std::string& config, IBrockerCallback *callback) override;

    void sendNotification(long long int timestamp, int level, const std::string &eventId, const std::string &sender,
                          const std::string &type, DataBlock *data) override;

    void sendErrorEvent(long long int timestamp, int errorCode, const std::string& message, bool stopPropagation) override;

    void sendActionResponse(long long int timestamp, const std::string &type, const std::string &sender, DataBlock *data) override;

    void sendActionRequest(long long int timestamp, const std::string& serviceId, const std::string &type, const std::string &sender, DataBlock *data) override;

    void subscribe(const std::string& serviceId) override;

    void unsubscribe(const std::string& serviceId) override;

    void sendConfigNotification(const std::string &sender, const std::string &data, bool stopPropagation) override;
    
    void sub_controller() override;

protected:
    //клиент MQTT
    mosquitto *m_mqttClient = nullptr;

    /**
     * обработчик обратного вызова
     */
    IBrockerCallback *m_callback = nullptr;

    std::string m_serviceId{}; // собственный id

    std::map<std::string, CHECK_EXIST_SUB_RES> m_Sub_TopicAndCheck{}; /* Топики, на которые мы подписаны + 
                                                                             результат проверки существования подписки*/ 

    /**
 * обработка подключения к брокеру MQTT
 * @param instance экземпляр клиента MQTT
 * @param obj пользовательские данные
 * @param status статус подключения:
 * * 0 - success
 * * 1 - connection refused (unacceptable protocol version)
 * * 2 - connection refused (identifier rejected)
 * * 3 - connection refused (broker unavailable)
 * * 4-255 - reserved for future use
 */
    static void mqtt_on_connect(struct mosquitto *instance, void * obj, int status);

    /**
     * обработка отключения от брокера
     * @param instance экземпляр клиента MQTT
     * @param obj пользовательские данные
     * @param status статус подключения
     */
    static void mqtt_on_disconnect(struct mosquitto *instance, void *obj, int status);

    /**
     * обработка отправки сообщения в брокер
     * @param instance экземпляр клиента MQTT
     * @param obj пользовательские данные
     * @param status статус подключения
     */
    static void mqtt_on_publish(struct mosquitto *instance, void *obj, int status);

    /**
     * обработка получения сообщения с брокера
     * @param instance экземпляр клиента MQTT
     * @param obj пользовательские данные
     * @param message сообщение
     */
    static void mqtt_on_message(struct mosquitto *instance, void *obj, const struct mosquitto_message *message);

    /**
     * обработка отладочного вывода
     * @param instance экземпляр клиента MQTT
     * @param obj пользовательские данные
     * @param level уровень сообщения
     * @param message сообщение
     */
    static void on_log(struct mosquitto *instance, void *obj, int level, const char *message);
};
#endif //ROOT_OR_SERVER_MONITOR_MODULE_MQTTBROCKER_H
