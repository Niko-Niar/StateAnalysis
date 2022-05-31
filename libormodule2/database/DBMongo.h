//
// Created by kulakov on 06.04.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_DBMONGO_H
#define ROOT_OR_SERVER_MONITOR_MODULE_DBMONGO_H

#include <uv.h>
#include "IDatabase.h"

class DBMongo : public IDatabase {
public:
    ~DBMongo() override = default;

    void init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) override;

    void updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) override;

    ModuleDescription getServiceName(const std::string &serviceID) override;

    void getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    std::string storeData(long long timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data) override;

    void storeError(const std::string &data) override;

    void sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) override;

    void updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    void createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) override;

    void getConfigsByType(const std::string &dataType, const DataBlock &block) override;

    void requestData(DataQuery pQuery) override;

    std::vector<std::string> getServiceList() override;

    int removeData(std::string serviceId, std::string messageType, long long timestamp) override;

    void checkStatus() override;

protected:
    static void timer_callback_getConfig(uv_timer_t *timer);

    static void timer_callback_getConfigsByType(uv_timer_t *timer);

    IDatabaseCallback *m_callback = nullptr;

    std::string m_mongoDbUri{};
    std::string m_database{};

    /**
     * таймер для перевода чтения данных с БД в каллбаки
     */
    uv_timer_t m_callbackTimer{};

};


#endif //ROOT_OR_SERVER_MONITOR_MODULE_DBMONGO_H
