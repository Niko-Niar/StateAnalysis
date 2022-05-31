//
// Created by kulakov on 07.04.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_DATABLOCK_H
#define ROOT_OR_SERVER_MONITOR_MODULE_DATABLOCK_H


#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <typeinfo>

enum OR_DATA_TYPES {
    OR_TYPE_STRING, // строка до 16383 байт
    OR_TYPE_JSON, //строка в формате json
    OR_TYPE_INT, // целое число
    OR_TYPE_LONG, // длинное целое число
    OR_TYPE_FLOAT, // вещественное число
    OR_TYPE_DOUBLE, // вещественное число двойной точности
    OR_TYPE_ARRAY, // массив
    OR_TYPE_BOOL // логический тип
};

class DataBlock {
public:
    virtual ~DataBlock() = default;

    /**
     * Получение списка полей
     * @return список названий полей
     */
    std::vector<std::string> getItems() const;

    /**
     * получение значения поля по ключу в формате json
     * @param key название поля
     * @return значение поля в строковом формате
     * @throws std::runtime_error - если не найден ключ
     */
    std::string getString(const std::string &key, const std::string &default_value = std::string()) const noexcept(false);

    /**
     * Получение типа поля по ключу
     * @param key название поля
     * @return тип поля
     * @throws std::runtime_error - ели не найден ключ
     */
    OR_DATA_TYPES getItemType(const std::string &key) const noexcept(false);

    bool getBool(const std::string &key, bool default_value = false) const;

    int getInt(const std::string &key, int default_value = 0) const;

    long getLong(const std::string &key, long default_value = 0) const;

    float getFloat(const std::string &key, float default_value = 0) const;

    double getDouble(const std::string &key, double default_value = 0) const;

    std::vector<double> getArray(const std::string &key) const;

    bool hasValue(const std::string &key) const;

    void setValue(const std::string &key, const std::string &value);
    void setValue(const std::string &key, const char *value);
    void setValue(const std::string &key, bool value);
    void setValue(const std::string &key, int value);
    void setValue(const std::string &key, unsigned int value);
    void setValue(const std::string &key, long value);
    void setValue(const std::string &key, double value);
    void setValue(const std::string &key, const std::vector<double>& value);

    void setValueByType(const std::string &key, const std::string &value);
    void setValueByType(const std::string &key, long value);
    void setValueByType(const std::string &key, int value);
    void setValueByType(const std::string &key, double value);

    std::string getFilledItems() const;

    std::string toJson() const;

    bool parseFromJson(const std::string &content);

    void pop(const std::string& key);

    void clear();

    virtual DataBlock *createInstance() const {
        throw std::runtime_error(std::string("DataBlock::createInstance() NOT IMPLEMENTED IN CHILD CLASS ") + typeid(*this).name() + "!!!!");
    };

    long long int getBlockTimestamp();

    void setBlockTimestamp(long long timestamp);

protected:
    virtual std::map<std::string, OR_DATA_TYPES> getBlockDescription() const {
        throw std::runtime_error(std::string("DataBlock::getBlockDescription() NOT IMPLEMENTED IN CHILD CLASS ") + typeid(*this).name() + "!!!!");
    }

    std::map<std::string, std::string> m_stringValues{};
    std::map<std::string, bool> m_boolValues{};
    std::map<std::string, int> m_intValues{};
    std::map<std::string, float> m_floatValues{};
    std::map<std::string, long> m_longValues{};
    std::map<std::string, double> m_doubleValues{};
    std::map<std::string, std::vector<double>> m_arrayDoubleValues{};

    static std::string getTypeName(OR_DATA_TYPES type);

    /**
     * Временная метка блока. Используется для передачи результатов запроса
     */
    long long m_timestamp = 0;
};


#endif //ROOT_OR_SERVER_MONITOR_MODULE_DATABLOCK_H
