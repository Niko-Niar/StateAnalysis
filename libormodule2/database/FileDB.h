//
// Created by kulakov on 01.07.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_FILEDB_H
#define ROOT_OR_SERVER_MONITOR_MODULE_FILEDB_H

#include <uv.h>

#include <utility>
#include "IDatabase.h"

class file_callback_data {
public:
    explicit file_callback_data(IDatabaseCallback *callback) {
        m_callback = callback;
    }

    file_callback_data(const file_callback_data& other) {
        m_guid = other.m_guid;
        m_block = other.m_block;
        m_types = other.m_types;
        m_lists = other.m_lists;
        m_callback = other.m_callback;
    }

    void setBlock(DataBlock *block) {
        m_block = block;
    }

    void append(const std::string& type, std::map<std::string, DataBlock*> list) {
        m_types.push_back(type);
        m_lists[type] = std::move(list);
    }

    void setGuid(const std::string &guid) {
        m_guid = guid;
    }

    void processBlock() {
        if (m_callback)
            m_callback->databaseGotConfig(m_guid, m_block);
    }

    void processList() {
        if (m_callback) {
            for (const auto &item: m_types) {
                m_callback->databaseGotOtherConfigs(item, m_lists[item]);
            }
        }
    }

    file_callback_data& operator=(const file_callback_data & other) = default;

protected:
    std::string m_guid{};
    DataBlock *m_block = nullptr;
    std::vector<std::string> m_types{};
    std::map<std::string, std::map<std::string, DataBlock *>> m_lists{};
    IDatabaseCallback *m_callback = nullptr;
};

class FileDB : public IDatabase {
public:
    void init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string& config, IDatabaseCallback *callback) override;

    void updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string& status, bool stopPropagation) override;

    ModuleDescription getServiceName(const std::string &serviceID) override;

    void getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    void sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) override;

    std::string storeData(long long timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type,  DataBlock *data) override;

    void storeError(const std::string &data) override;

    void updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock*) override;

    void createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) override;

    void getConfigsByType(const std::string &dataType, const DataBlock &block) override;

    void requestData(DataQuery pQuery) override;

    std::vector<std::string> getServiceList() override;

    int removeData(std::string serviceId, std::string messageType, long long timestamp) override;

    void checkStatus() override;

protected:
    static void timer_callback_getConfig(uv_timer_t *timer);

    static void timer_callback_getConfigsByType(uv_timer_t *timer);

    std::string m_cluster{};
    IDatabaseCallback *m_callback = nullptr;
    std::string m_path{};

    /**
     * таймер для перевода чтения данных с БД в каллбаки
     */
    uv_timer_t m_callbackTimer{};

};


#endif //ROOT_OR_SERVER_MONITOR_MODULE_FILEDB_H
