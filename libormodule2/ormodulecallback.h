
#pragma once

#include <string>
#include "DataBlock.h"
#include "datatypes/DataQuery.h"

/**
 * Интерфейс для обработчиков событий из подключенных баз данных
 */
class ORModuleCallback {

public:
    virtual ~ORModuleCallback() = default;

    /**
     * обработка инициализации соединения со всеми БД
     */
    virtual void orOnReady() = 0;

    /**
     * обработка получения конфига с БД
     * @param config заполненная структура конфига
     */
    virtual void orGotConfig(const std::string &guid, DataBlock *config) = 0;

    /**
     * обработка получения событий от web клиента
     * @param doc сообщение в формате json
     */
    virtual void orGotAction(const std::string &doc);

    /**
     * обработка получения уведомления от другого сервиса
     * @param serviceId ИД сервиса
     * @param message текст сообщения в формате json
     */
    virtual void orGotEvent(const std::string &serviceId, const std::string &message);

    /**
     * обработка и отправка события каждые NN секунд
     * @param level уровень индексации
     */
    virtual void orOnUpdate() = 0;

    /**
     * обработка ошибок из БД
     * @param errorCode код ошибки
     * @param errorMessage текст ошибки
     */
    virtual void orGotError(int errorCode, const std::string &errorMessage) = 0;

    /**
     * обработка получения конфигов сторонних модулей
     * @param dataType тип модуля(ей)
     * @param vector перечень конфигов модуля(ей)
     */
    virtual void orGotOtherConfigs(const std::string &dataType, const std::map<std::string, DataBlock *>& vector);

    /**
     * Получение очередной порции данных с БД
     * @param query параметры запроса
     * @param content новый экземпляр объекта данных. Необходимо удалить после использования
     */
    virtual void orGotStorageData(long long timestamp, DataQuery query, DataBlock *content);
};

