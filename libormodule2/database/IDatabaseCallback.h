//
// Created by kulakov on 06.04.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_IDATABASECALLBACK_H
#define ROOT_OR_SERVER_MONITOR_MODULE_IDATABASECALLBACK_H

#include "../DataBlock.h"
#include "../datatypes/DataQuery.h"

class IDatabase;

class IDatabaseCallback {
public:
    virtual ~IDatabaseCallback() = default;

    virtual void databaseGotConfig(const std::string &guid, DataBlock *config) = 0;

    /**
     * Сообщение об ошибке внутри адаптера к БД
     * @param instance экземпляр адаптера
     * @param errorMessage текст ошибки
     */
    virtual void databaseGotError(IDatabase *instance, const std::string &errorMessage) = 0;

    /**
     * Получение конфигов перечня сторонних модулей (или одного модуля) одного типа
     * @param vector перечень конфигов
     */
    virtual void databaseGotOtherConfigs( const std::string &dataType, const std::map<std::string, DataBlock *>& vector) = 0;

    /**
     * Оправка полученного результата запроса к БД в библиотеку
     * @param query запрос
     * @param result результат
     */
    virtual void databaseGotData(DataQuery query, DataBlock *result) = 0;
};
#endif //ROOT_OR_SERVER_MONITOR_MODULE_IDATABASECALLBACK_H
