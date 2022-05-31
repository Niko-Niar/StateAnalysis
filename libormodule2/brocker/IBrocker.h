//
// Created by kulakov on 25.03.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_IBROCKER_H
#define ROOT_OR_SERVER_MONITOR_MODULE_IBROCKER_H

#include "IBrockerCallback.h"

#include "../DataBlock.h"

#include <string>

/**
 * класс-интерфейс для обмена сообщениями
 */
class IBrocker {
public:
    virtual ~IBrocker() = default;

    /**
     * настройка брокера сообщений
     * @param serviceId идентификатор сервиса
     * @param config конфиг в формате json
     * @param callback обработчик обратных вызовов брокера сообщений
     */
    virtual void init(const std::string& clusterName, const std::string& serviceId, const std::string& config, IBrockerCallback *callback) = 0;

    /**
     * Отправка уведомления в брокер
     * @param timestamp временная метка
     * @param type универсальный тип сообщения
     * @param level уровень сообщения
     * @param eventId идентификатор события если есть
     * @param sender идентификатор отправителя
     * @param data данные события
     */
    virtual void sendNotification(long long int timestamp, int level, const std::string &eventId,
                                  const std::string &sender, const std::string &type, DataBlock *data) = 0;

    virtual void sendErrorEvent(long long int timestamp, int errorCode, const std::string& message, bool stopPropagation) = 0;

    /**
     * Отправка ответа на действие
     * @param timestamp временная метка
     * @param type тип действия
     * @param sender отправитель
     * @param data данные
     */
    virtual void sendActionResponse(long long int timestamp, const std::string &type, const std::string &sender, DataBlock *data) = 0;

    /**
     * Отправка запроса на действие
     * @param timestamp временная метка
     * @param serviceId идентификатор сервиса получателя
     * @param type тип действия
     * @param sender идентификатор отправителя
     * @param data данные запроса
     */
    virtual void sendActionRequest(long long int timestamp, const std::string& serviceId, const std::string &type, const std::string &sender, DataBlock *data) = 0;

    virtual void subscribe(const std::string& serviceId) = 0;

    virtual void unsubscribe(const std::string& serviceId) = 0;

    virtual void sub_controller() = 0; 

    /**
     * Отправка уведомления об изменении конфигурации модуля (статус, параметры настройки и т.п.)
     */
    virtual void sendConfigNotification(const std::string &sender, const std::string &data, bool stopPropagation) = 0;

    static IBrocker *getBrocker(const std::string & brokerName);
};

#endif //ROOT_OR_SERVER_MONITOR_MODULE_IBROCKER_H
