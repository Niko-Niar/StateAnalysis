//
// Created by kulakov on 13.04.2021.
//

#ifndef ORMODULE2_ORMODULE_H
#define ORMODULE2_ORMODULE_H

#include "DataBlock.h"
#include "ormodulecallback.h"
#include "datatypes/ModuleDescription.h"
#include "datatypes/DataQuery.h"

#include <string>
#include <uv.h>

/**
 * путь по умолчанию до файла конфигурации
 */
#ifndef OR_DEFAULT_CONFIG_PATH
#define OR_DEFAULT_CONFIG_PATH "/etc/optirepair/config.json"
#endif

/**
 * путь по умолчанию до хранилища файлов
 */
#ifndef OR_DEFAULT_STORAGE_PATH
#define OR_DEFAULT_STORAGE_PATH "/var/opt/optirepair/storage"
#endif

/**
 * таймаут 3 минуты (уровень 2)
 */
#ifndef OR_DEFAULT_TIMEOUT
#define OR_DEFAULT_TIMEOUT 60 * 3
#endif

class ORModuleImpl;

class ORModule {
public:
    // запрет копирования объекта
    ORModule(const ORModule& value) = delete;
    void operator=(const ORModule& value) = delete;

    /**
     * инициализация модуля
     * @param moduleType тип модуля, используется для определения места хранения
     * @param service_id ID модуля
     * @param cb обработчик функций обратного вызова модуля
     */
    static void init(const std::string& moduleType, const std::string &service_id, ORModuleCallback *cb);

    /**
     * инициализация модуля
     * @param moduleType тип модуля, используется для определения места хранения
     * @param service_id ID модуля
     * @param cb обработчик функций обратного вызова модуля
     * @param config_path путь до файла с конфигурацией системы
     */
    static void init(const std::string& moduleType, const std::string &service_id, ORModuleCallback *cb, const std::string &config_path);

    /**
     * очистка модуля, закрытие обработчиков, остановка таймера, ...
     */
    static void cleanup();

    /**
     * отправка ошибки в систему
     * @param errorCode код ошибки
     * @param errorMessage текст ошибки
     */
    static void sendErrorEvent(int errorCode, const std::string &errorMessage, bool stopPropagation = false);

    /**
     * Отправка ответа на запрос действия
     * @param timestamp временная метка
     * @param type тип запроса
     * @param sender идентификатор отправителя
     * @param data данные ответа
     */
    static void sendActionResponse(long long timestamp, const std::string &type, const std::string &sender, DataBlock *data);

    /**
     * Отправка запроса на действие другому модулю
     * @param timestamp временная метка
     * @param serviceId идентификатор модуля приемника
     * @param type тип запроса
     * @param sender идентификатор отправителя
     * @param data данные запроса
     */
    static void sendActionRequest(long long timestamp, const std::string &serviceId, const std::string &type, const std::string &sender, DataBlock *data);

    /**
     * Отправка уведомления в брокер без сохранения данных
     * @param timestamp временная метка
     * @param type тип сообщения
     * @param level уровень сообщения
     * @param sender отправитель
     * @param data данные в формате DataBlock
     */
    static void sendNotification(long long timestamp, int level, const std::string &sender, const std::string &type, DataBlock *data);

    /**
     * Запись события в БД
     * @param timestamp временная мета
     * @param type тип события
     * @param level уровень важности события (0-3)
     * @param senderType тип модуля отправителя
     * @param senderId идентификатор модуля отправителя
     * @param data данные события в формате DataBlock
     */
    static void storeEvent(long long timestamp, int level, const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data);

    /**
     * Запись оперативных данных в БД
     * @param timestamp временная метка
     * @param type тип оперативных данных
     * @param senderType тип модуля отправителя
     * @param senderId идентификатор модуля отправителя
     * @param data данные для сохранения
     */
    static void storeSourceData(long long timestamp, const std::string &senderType, const std::string &senderId,
                                const std::string &type, DataBlock *data,
                                const std::vector<std::string>& exclude_fields = {});

    /**
     * установка статуса модуля
     * @param status название статуса
     */
    static void updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation = false);

    /**
     * получение конфига.
     * @see ORModuleCallback::orGotConfig()
     */
    static void getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block);

    /**
     * получение собственного ID сервиса
     * @return ID сервиса
     */
    static std::string getServiceId();

    /**
     * получение названия модуля/сервиса
     * @param serviceId идентификатор модуля/сервиса
     * @return название сервиса
     */
    static ModuleDescription getServiceName(const std::string &serviceId);

    /**
     * получение имени узла
     * @return имя узла
     */
    static std::string getClusterName();

    static std::string getStorageDir();

    static std::string getSettingsPath();

    static void subscribeToService(const std::string &serviceId);

    static void unsubscribeToService(const std::string &serviceId);

    static void sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config);

    /**
     * Обновление настроек модуля самим модулем
     * @param block перечень измененных настроек
     */
    static void addConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block);

    /**
     * Получение перечня конфигов по типу
     * @param type тип модуля
     * @param pBlock инстанс блока для создания конфигов
     */
    static void getConfigsByType(const char *type, const DataBlock &pBlock);

    static std::string getModuleType();

    static void close();

    /**
     * Отправка запроса в систему на получение оперативных данных
     * @see ModuleCallback::gotSourceData()
     * @param query запрос
     */
    static void requestData(DataQuery query);

    static std::vector<std::string> getServiceList();

    static int removeFileData(const std::string& serviceId, const std::string& messageType, long long int timestamp);

    static int removeSourceData(const std::string& serviceId, const std::string& messageType, long long int timestamp);

    static int removeEventData(const std::string& serviceId, const std::string& messageType, long long int i);

protected:
    static ORModuleImpl *getInstance();

    static ORModuleImpl * m_instance;

    // таймер для циклических вызовов
    static uv_timer_t *timer_req;

    /**
     * обработка обновления каждые 5 минут
     */
    static void r_onUpdate(uv_timer_s* timer);

    /**
     * обработчик закрытия таймера
     * @param handle
     */
    static void on_timer_close_complete(uv_handle_t *handle);

};


#endif //ORMODULE2_ORMODULE_H
