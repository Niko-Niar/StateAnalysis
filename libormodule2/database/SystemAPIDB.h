//
// Created by kulakov on 08.07.2021.
//

#ifndef OR_ADC_SOURCE_MODULE_SYSTEMAPIDB_H
#define OR_ADC_SOURCE_MODULE_SYSTEMAPIDB_H

#include "IDatabase.h"

#include <curl/curl.h>
#include <uv.h>

#include <utility>

struct systemapi_callback_data {
    explicit systemapi_callback_data(IDatabaseCallback *callback) {
        m_callback = callback;
    }

    systemapi_callback_data(const systemapi_callback_data& other) {
        m_callback = other.m_callback;
        m_block = other.m_block;
        m_lists = other.m_lists;
        m_types = other.m_types;
        m_guid = other.m_guid;
        m_query = other.m_query;
    }

    void setBlock(DataBlock *block) {
        m_block = block;
    }

    void setQuery(DataQuery query) {
        m_query = std::move(query);
    }

    void setGuid(const std::string &guid) {
        m_guid = guid;
    }

    void append(const std::string& type, std::map<std::string, DataBlock*> list) {
        m_types.push_back(type);
        m_lists[type] = std::move(list);
    }

    void processConfig() {
        m_callback->databaseGotConfig(m_guid, m_block);
    }

    void processList() {
        for(const auto& item : m_types) {
            m_callback->databaseGotOtherConfigs(item, m_lists[item]);
        }
    }

    void processQueryData() {
        m_callback->databaseGotData(m_query, m_block);
    }

    systemapi_callback_data& operator=(const systemapi_callback_data& other) = default;

protected:
    std::string m_guid{};
    DataBlock *m_block = nullptr;
    DataQuery m_query{};
    std::vector<std::string> m_types{};
    std::map<std::string, std::map<std::string, DataBlock *>> m_lists{};
    IDatabaseCallback *m_callback = nullptr;
};

class SystemAPIDB : public IDatabase {
public:
    void init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string& config, IDatabaseCallback *callback) override;

    void updateStatus( const std::string& moduleType, const std::string& serviceId, const std::string& status, bool stopPropagation) override;

    ModuleDescription getServiceName(const std::string &serviceID) override;

    void storeError(const std::string &data) override;

    void getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    void updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) override;

    void sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) override;

    std::string storeData(long long timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data) override;

    void createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) override;

    void getConfigsByType(const std::string &dataType, const DataBlock &block) override;

    void requestData(DataQuery pQuery) override;

    std::vector<std::string> getServiceList() override;

    int removeData(std::string serviceId, std::string messageType, long long timestamp) override;

    void checkStatus() override;

protected:
    /**
     * Запись ответа от сервера в переменную
     * @param ptr
     * @param size
     * @param nmemb
     * @param data
     * @return
     */
    static size_t write_data(char *ptr, size_t size, size_t nmemb, std::string* data);

    /**
     * Асинхронная отправка конфига
     * @param timer таймер
     */
    static void timer_callback_getConfig(uv_timer_t *timer);

    static void timer_callback_getConfigsByType(uv_timer_t *timer);

    /**
     * Асинхронная отправка результата запроса
     * @param timer таймер
     */
    static void timer_callback_getData(uv_timer_t *timer);

    std::string m_cluster{};
    IDatabaseCallback *m_callback = nullptr;

    std::string m_host{};
    CURL *m_curl = nullptr;

    /**
     * таймер для перевода чтения данных с БД в каллбаки
     */
    uv_timer_t m_callbackTimer{};

    long error_count = 0;
    long lastErrorNum = 0;
};


#endif //OR_ADC_SOURCE_MODULE_SYSTEMAPIDB_H
