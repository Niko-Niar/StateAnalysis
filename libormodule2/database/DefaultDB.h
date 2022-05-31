//
// Created by kulakov on 14.04.2021.
//

#ifndef ORMODULE2_TESTS_DEFAULTDB_H
#define ORMODULE2_TESTS_DEFAULTDB_H

#include "IDatabase.h"

class DefaultDB : public IDatabase {
public:
    ~DefaultDB() override;

    void init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string& config, IDatabaseCallback *callback) override;

    void updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string& status, bool stopPropagation) override;

    ModuleDescription getServiceName(const std::string &serviceID) override;

    void storeError(const std::string &data) override;

    void getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    std::string storeData(long long int timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type,
                   DataBlock *data) override;

    void sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) override;

    void updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    void createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) override;

    void getConfigsByType(const std::string &dataType, const DataBlock &block) override;

    void requestData(DataQuery pQuery) override;

    std::vector<std::string> getServiceList() override;

    int removeData(std::string serviceId, std::string messageType, long long timestamp) override;

    void checkStatus() override;
};


#endif //ORMODULE2_TESTS_DEFAULTDB_H
