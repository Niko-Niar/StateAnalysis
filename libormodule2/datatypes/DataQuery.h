//
// Created by kulakov on 09.09.2021.
//

#ifndef OR_BEARING_CONTROL_DATAQUERY_H
#define OR_BEARING_CONTROL_DATAQUERY_H

class DataQuery {
public:
    void setType(const std::string &type) {
        m_type = type;
    }

    std::string getType() const {
        return m_type;
    }

    void setModuleType(const std::string &type) {
        m_moduleType = type;
    }

    std::string getModuleType() const {
        return m_moduleType;
    }

    void setModuleId(const std::string &type) {
        m_moduleId = type;
    }

    std::string getModuleId() const {
        return m_moduleId;
    }

    void setBlock(DataBlock *block) {
        m_block = block;
    }

    DataBlock * getBlock() const {
        return m_block;
    }

    void setTimestamp(long long timestamp) {
        m_timestamp = timestamp;
    }

    long long getTimestamp() const {
        return m_timestamp;
    }

    /**
     * Определение доп.параметров фильтрации
     * @param filter параметры фильтрации в формате json
     */
    void setFilter(const std::string &filter) {
        m_filter = filter;
    }

    std::string getFilter() const {
        return m_filter;
    }

protected:
    // тип данных
    std::string m_type{};
    // тип модуля
    std::string m_moduleType{};
    // guid модуля
    std::string m_moduleId{};
    // фабрика данных запроса
    DataBlock *m_block = nullptr;

    // фильтры запроса в формате json
    std::string m_filter{};

    // временная метка
    long long m_timestamp = 0;
};

#endif //OR_BEARING_CONTROL_DATAQUERY_H
