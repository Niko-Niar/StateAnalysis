//
// Created by kulakov on 06.04.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_IDATABASE_H
#define ROOT_OR_SERVER_MONITOR_MODULE_IDATABASE_H

#include "IDatabaseCallback.h"
#include "../DataBlock.h"
#include "../datatypes/ModuleDescription.h"
#include "../datatypes/DataQuery.h"

#include <string>

/**
 * Класс-интерфейс для сохранения данных в БД
 */
class IDatabase {
public:

    virtual ~IDatabase() = default;

    /**
     * настройка брокера сообщений
     * @param config конфиг в формате json
     * @param callback обработчик обратных вызовов брокера сообщений
     */
    virtual void init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string& config, IDatabaseCallback *callback) = 0;

    /**
     * Обновление статуса модуля (вариант БД настроек)
     * @param status новый статус модуля
     */
    virtual void updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string& status, bool stopPropagation) = 0;

    /**
     * Получение имени сервиса (вариант БД настроек)
     * @param serviceID идентификатор сервиса
     * @return имя сервиса или пустая строка
     */
    virtual ModuleDescription getServiceName(const std::string &serviceID) = 0;

    /**
     * запрос на получение конфига (вариант БД настроек)
     */
    virtual void getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) = 0;

    /**
     * Сохранение ошибки (вариант БД событий)
     * @param data данные ошибки
     */
    virtual void storeError(const std::string &data) = 0;

    /**
     * Сохранение данных в БД (вариант БД событий и БД рядов)
     * @param timestamp метка времени
     * @param useLevel требуется ли сохранять поле level, используется для событий
     * @param level уровень события или 0
     * @param type тип события
     * @param sender идентификатор сервиса
     * @param data данные
     * @return идентификатор записи или пустая строка
     */
    virtual std::string storeData(long long int timestamp, bool useLevel, int level, const std::string &senderType,
                                  const std::string &senderId, const std::string &type, DataBlock *data) = 0;

    /**
     * Сохранение параметров конфигурации для настройки модуля
     * @param config Параметры конфигурации в виде json файла
     */
    virtual void sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) = 0;

    /**
     * Обновление конфигурации самим модулем
     * @param block блок конфигурации с обновлениями
     */
    virtual void updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) = 0;

    /**
     * Создание таблицы/хранилища если ее нет
     * @param tableName имя таблицы
     * @param block поля таблицы
     */
    virtual void createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) = 0;

    static IDatabase * getDatabase(const std::string &type);

    virtual void getConfigsByType(const std::string &dataType, const DataBlock &block) = 0;

    /**
     * Отправка запроса к хранилищу оперативных данных
     * @param pQuery параметры запроса
     */
    virtual void requestData(DataQuery pQuery) = 0;

    virtual std::vector<std::string> getServiceList() = 0;

    /**
     * Очистка БД от старых записей
     * @param serviceId идентификатор модуля
     * @param messageType тип сообщения
     * @param timestamp временная метка
     * @return число удаленных записей
     */
    virtual int removeData(std::string serviceId, std::string messageType, long long int timestamp) = 0;

    /**
     * периодическая проверка соединения с БД
     */
    virtual void checkStatus() = 0;
};

#endif //ROOT_OR_SERVER_MONITOR_MODULE_IDATABASE_H
