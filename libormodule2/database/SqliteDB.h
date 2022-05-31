//
// Created by kulakov on 01.07.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_SQLITEDB_H
#define ROOT_OR_SERVER_MONITOR_MODULE_SQLITEDB_H

#include "IDatabase.h"

#include <sqlite3.h> //libsqlite3-dev

class SqliteDB : public IDatabase {
public:
    void init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string& config, IDatabaseCallback *callback) override;

    void storeError(const std::string &data) override;

    std::string storeData(long long timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type,  DataBlock *data) override;

    void sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) override;

    void getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    ModuleDescription getServiceName(const std::string &serviceID) override;

    void updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) override;

    void updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock*) override;

    void createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) override;

    void getConfigsByType(const std::string &dataType, const DataBlock &block) override;

    void requestData(DataQuery pQuery) override;

    std::vector<std::string> getServiceList() override;

    int removeData(std::string serviceId, std::string messageType, long long timestamp) override;

    void checkStatus() override;

protected:
    std::string m_cluster{};
    IDatabaseCallback *m_callback = nullptr;
    std::string m_path{};

    sqlite3 *m_db = nullptr;

    static int busy_callback(void * stmt, int times);
};


#endif //ROOT_OR_SERVER_MONITOR_MODULE_SQLITEDB_H
