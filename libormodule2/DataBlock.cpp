//
// Created by kulakov on 07.04.2021.
//

#include <nlohmann/json.hpp>
#include "DataBlock.h"
#include "ordebug.h"
#include "orutils.h"

std::vector<std::string> DataBlock::getItems() const {
    auto desc = this->getBlockDescription();
    std::vector<std::string> ret;
    ret.reserve(desc.size());
for(auto & it : desc) {
        ret.push_back(it.first);
    }
    return ret;
}

std::string DataBlock::getString(const std::string &key, const std::string &default_value) const noexcept(false) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getString(): KEY " + key + " NOT FOUND!!!");
    }
    auto type = pos->second;

    switch (type) {
        case OR_TYPE_INT: {
            auto intpos = m_intValues.find(key);
            if (intpos == m_intValues.end()) {
                return default_value;
            } else {
                return std::to_string(intpos->second);
            }
        }
        case OR_TYPE_LONG: {
            auto longpos = m_longValues.find(key);
            if (longpos == m_longValues.end()) {
                return default_value;
            } else {
                return std::to_string(longpos->second);
            }
        }
        case OR_TYPE_FLOAT: {
            auto floatpos = m_floatValues.find(key);
            if (floatpos == m_floatValues.end()) {
                return default_value;
            } else {
                return std::to_string(floatpos->second);
            }
        }
        case OR_TYPE_DOUBLE: {
            auto doublepos = m_doubleValues.find(key);
            if (doublepos == m_doubleValues.end()) {
                return default_value;
            } else {
                return std::to_string(doublepos->second);
            }
        }
        case OR_TYPE_STRING:
        case OR_TYPE_JSON: {
            auto strpos = m_stringValues.find(key);
            if (strpos == m_stringValues.end()) {
                return default_value;
            } else {
                return strpos->second;
            }
        }
        case OR_TYPE_ARRAY: {
            auto arraypos = m_arrayDoubleValues.find(key);
            if (arraypos == m_arrayDoubleValues.end()) {
                return default_value;
            } else {
                nlohmann::json value(arraypos->second);
                return std::string(value.dump());
            }
        }
        default:
            throw std::runtime_error("DataBlock::getString(): NOT IMPLEMENTED FOR KEY " + key + " and type=" + getTypeName(type));
    }
}

OR_DATA_TYPES DataBlock::getItemType(const std::string &key) const noexcept(false) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getItemType(): KEY " + key + " NOT FOUND!!!");
    }
    return pos->second;
}

int DataBlock::getInt(const std::string &key, int default_value) const {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getInt(): KEY "+ key + " NOT FOUND!!!");
    }
    if (pos->second != OR_TYPE_INT) {
        throw std::runtime_error("DataBlock::getInt(): WRONG TYPE for key " + key + ", REQUIRED INT");
    }
    auto intpos = m_intValues.find(key);
    if (intpos == m_intValues.end()) {
        return default_value;
    }
    return intpos->second;
}

long DataBlock::getLong(const std::string &key, long default_value) const {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getLong(): KEY " + key + " NOT FOUND!!!");
    }
    if (pos->second != OR_TYPE_LONG) {
        throw std::runtime_error("DataBlock::getLong(): WRONG TYPE, REQUIRED LONG");
    }
    auto intpos = m_longValues.find(key);
    if (intpos == m_longValues.end()) {
        return default_value;
    }
    return intpos->second;
}

bool DataBlock::getBool(const std::string &key, bool default_value) const {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getBool(): KEY " + key + " NOT FOUND!!!");
    }
    if (pos->second != OR_TYPE_BOOL) {
        throw std::runtime_error("DataBlock::getBool(): WRONG TYPE, REQUIRED BOOL");
    }
    auto intpos = m_boolValues.find(key);
    if (intpos == m_boolValues.end()) {
        return default_value;
    }
    return intpos->second;
}

float DataBlock::getFloat(const std::string &key, float default_value) const {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getFloat(): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second == OR_TYPE_FLOAT) {
        auto intpos = m_floatValues.find(key);
        if (intpos == m_floatValues.end()) {
            return default_value;
        }
        return intpos->second;
    }

    throw std::runtime_error("DataBlock::getFloat(): WRONG TYPE, REQUIRED FLOAT");
}

double DataBlock::getDouble(const std::string &key, double default_value) const {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getDouble(): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second == OR_TYPE_DOUBLE) {
        auto intpos = m_doubleValues.find(key);
        if (intpos == m_doubleValues.end()) {
            return default_value;
        }
        return intpos->second;
    }
    if (pos->second == OR_TYPE_FLOAT) {
        auto intpos = m_floatValues.find(key);
        if (intpos == m_floatValues.end()) {
            return default_value;
        }
        return intpos->second;
    }

    throw std::runtime_error("DataBlock::getDouble(): WRONG TYPE FOR KEY " + key + ", REQUIRED DOUBLE");
}

std::vector<double> DataBlock::getArray(const std::string &key) const {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::getArray(): KEY " + key + " NOT FOUND!!!");
    }
    if (pos->second != OR_TYPE_ARRAY) {
        throw std::runtime_error("DataBlock::getArray(): WRONG TYPE FOR KEY " + key + ", REQUIRED ARRAY OF DOUBLE");
    }
    auto intpos = m_arrayDoubleValues.find(key);
    if (intpos == m_arrayDoubleValues.end()) {
        return {};
    }
    return intpos->second;
}

bool DataBlock::hasValue(const std::string &key) const {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::hasValue(): KEY " + key + " NOT FOUND!!!");
    }
    switch (pos->second) {
        case OR_TYPE_INT: {
            auto intpos = m_intValues.find(key);
            if (intpos == m_intValues.end()) {
                return false;
            } else {
                return true;
            }
        }
        case OR_TYPE_JSON:
        case OR_TYPE_STRING: {
            auto strpos = m_stringValues.find(key);
            if (strpos == m_stringValues.end()) {
                return false;
            } else {
                return !m_stringValues.at(key).empty();
            }
        }
        case OR_TYPE_FLOAT: {
            auto ipos = m_floatValues.find(key);
            if (ipos == m_floatValues.end()) {
                return false;
            } else {
                return true;
            }
        }
        case OR_TYPE_DOUBLE: {
            auto ipos = m_doubleValues.find(key);
            if (ipos == m_doubleValues.end()) {
                return false;
            } else {
                return true;
            }
        }
        case OR_TYPE_BOOL: {
            auto ipos = m_boolValues.find(key);
            if (ipos == m_boolValues.end()) {
                return false;
            } else {
                return true;
            }
        }
        case OR_TYPE_LONG: {
            auto ipos = m_longValues.find(key);
            if (ipos == m_longValues.end()) {
                return false;
            } else {
                return true;
            }
        }
        case OR_TYPE_ARRAY: {
            auto ipos = m_arrayDoubleValues.find(key);
            if (ipos == m_arrayDoubleValues.end()) {
                return false;
            } else {
                return true;
            }
        }
        default:
            throw std::runtime_error("DataBlock::hasValue(): NOT IMPLEMENTED FOR KEY " + key + " and type=" +
                                     getTypeName(pos->second));
    }
}

void DataBlock::setValue(const std::string &key, const std::string &value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(string): KEY" + key + " NOT FOUND!!!");
    }

    if (pos->second == OR_TYPE_STRING || pos->second == OR_TYPE_JSON) {
        m_stringValues[key] = value;
        return;
    }

    throw std::runtime_error("DataBlock::setValue(string): WRONG TYPE FOR KEY " + key + ", REQUIRED STRING");
}

void DataBlock::setValue(const std::string &key, const char *value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(char*): KEY" + key + " NOT FOUND!!!");
    }

    if (pos->second == OR_TYPE_INT) {
        m_intValues[key] = atoi(value);
        return;
    }

    if (pos->second == OR_TYPE_LONG) {
        m_longValues[key] = atol(value);
        return;
    }

    if (pos->second != OR_TYPE_STRING && pos->second != OR_TYPE_JSON) {
        throw std::runtime_error("DataBlock::setValue(char*): WRONG TYPE FOR KEY " + key + ", REQUIRED STRING");
    }
    m_stringValues[key] = value;
}

void DataBlock::setValue(const std::string &key, bool value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(bool): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second != OR_TYPE_BOOL) {
        throw std::runtime_error("DataBlock::setValue(bool): WRONG TYPE FOR KEY " + key + ", REQUIRED BOOL");
    }
    m_boolValues[key] = value;
}

void DataBlock::setValue(const std::string &key, int value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(int): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second == OR_TYPE_INT) {
        m_intValues[key] = value;
        return;
    }
    if (pos->second == OR_TYPE_FLOAT) {
        m_floatValues[key] = static_cast<float>(value);
        return;
    }
    if (pos->second == OR_TYPE_LONG) {
        m_longValues[key] = value;
        return;
    }
    if (pos->second == OR_TYPE_DOUBLE) {
        m_doubleValues[key] = value;
        return;
    }

    throw std::runtime_error("DataBlock::setValue(int): WRONG TYPE FOR KEY " + key + ", REQUIRED INT");
}

void DataBlock::setValue(const std::string &key, unsigned int value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(uint): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second == OR_TYPE_INT) {
        m_intValues[key] = static_cast<int>(value);
        return;
    }
    if (pos->second == OR_TYPE_FLOAT) {
        m_floatValues[key] = static_cast<float>(value);
        return;
    }
    if (pos->second == OR_TYPE_LONG) {
        m_longValues[key] = value;
        return;
    }
    if (pos->second == OR_TYPE_DOUBLE) {
        m_doubleValues[key] = value;
        return;
    }

    throw std::runtime_error("DataBlock::setValue(uint): WRONG TYPE FOR KEY " + key + ", REQUIRED INT");
}

void DataBlock::setValue(const std::string &key, long value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(long): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second != OR_TYPE_LONG) {
        throw std::runtime_error("DataBlock::setValue(long): WRONG TYPE FOR KEY " + key + ", REQUIRED LONG");
    }
    m_longValues[key] = value;
}

void DataBlock::setValue(const std::string &key, double value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(double): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second == OR_TYPE_FLOAT) {
        m_floatValues[key] = static_cast<float>(value);
        return;
    }
    if (pos->second == OR_TYPE_DOUBLE) {
        m_doubleValues[key] = value;
        return;
    }

    throw std::runtime_error("DataBlock::setValue(double): WRONG TYPE FOR KEY " + key + ", REQUIRED DOUBLE OR FLOAT");
}

void DataBlock::setValue(const std::string &key, const std::vector<double>& value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValue(array): KEY " + key + " NOT FOUND!!!");
    }

    if (pos->second != OR_TYPE_ARRAY) {
        throw std::runtime_error("DataBlock::setValue(array): WRONG TYPE FOR KEY " + key + ", REQUIRED ARRAY OF DOUBLE");
    }
    m_arrayDoubleValues[key] = value;
}

void DataBlock::setValueByType(const std::string &key, const std::string &value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValueByType(string): KEY " + key + " NOT FOUND!!!");
    }

    if (value.empty()) {
        optirepair::ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Value for key " + key + " is empty");
        return;
    }

    switch (pos->second) {
        case OR_TYPE_STRING:
        case OR_TYPE_JSON:
            m_stringValues[key] = value;
            break;
        case OR_TYPE_INT:
            m_intValues[key] = std::stoi(value);
            break;
        case OR_TYPE_LONG:
            m_longValues[key] = std::stol(value);
            break;
        case OR_TYPE_DOUBLE:
            m_doubleValues[key] = optirepair::orutils::parseStringWithDouble(value);
            break;
        case OR_TYPE_FLOAT:
            m_floatValues[key] = static_cast<float>(optirepair::orutils::parseStringWithDouble(value));
            break;
        case OR_TYPE_BOOL:
            m_boolValues[key] = (value == "on" || value == "1");
            break;
        default:
            throw std::runtime_error("DataBlock::setValueByType(string): NOT IMPLEMENTED FOR KEY " + key + " and type=" + getTypeName(pos->second));
    }
}

void DataBlock::setValueByType(const std::string &key, long value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValueByType(long): KEY " + key + " NOT FOUND!!!");
    }

    switch (pos->second) {
        case OR_TYPE_INT: {
            if (value >= UINT32_MAX) {
                throw std::runtime_error("DataBlock::setValueByType(long): try to save long value as int ");
            }
            m_intValues[key] = static_cast<int>(value);
            optirepair::ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Required int, got long");
            break;
        }
        case OR_TYPE_DOUBLE:
            m_doubleValues[key] = static_cast<double>(value);
            break;
        case OR_TYPE_LONG:
            m_longValues[key] = value;
            break;
        case OR_TYPE_FLOAT:
            m_floatValues[key] = static_cast<float>(value);
            break;
        default:
            throw std::runtime_error("DataBlock::setValueByType(int): NOT IMPLEMENTED FOR KEY " + key + " and type=" + getTypeName(pos->second));
    }
}

void DataBlock::setValueByType(const std::string &key, int value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValueByType(int): KEY " + key + " NOT FOUND!!!");
    }

    switch (pos->second) {
        case OR_TYPE_INT:
            m_intValues[key] = value;
            break;
        case OR_TYPE_DOUBLE:
            m_doubleValues[key] = value;
            break;
        case OR_TYPE_LONG:
            m_longValues[key] = value;
            break;
        case OR_TYPE_FLOAT:
            m_floatValues[key] = static_cast<float>(value);
            break;
        default:
            throw std::runtime_error("DataBlock::setValueByType(int): NOT IMPLEMENTED FOR KEY " + key + " and type=" + getTypeName(pos->second));
    }
}

void DataBlock::setValueByType(const std::string &key, double value) {
    auto desc = this->getBlockDescription();
    auto pos = desc.find(key);
    if (pos == desc.end()) {
        throw std::runtime_error("DataBlock::setValueByType(double): KEY " + key + " NOT FOUND!!!");
    }

    switch (pos->second) {
        case OR_TYPE_INT:
            m_intValues[key] = static_cast<int>(value);
            optirepair::ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Required int, got double");
            break;
        case OR_TYPE_DOUBLE:
            m_doubleValues[key] = value;
            break;
        case OR_TYPE_FLOAT:
            m_floatValues[key] = static_cast<float>(value);
            break;
        case OR_TYPE_LONG:
            m_longValues[key] = static_cast<long>(value);
            optirepair::ordebug::warning(__PRETTY_FUNCTION__ , __LINE__, "Required long, got double");
            break;
        default:
            throw std::runtime_error("DataBlock::setValueByType(double): NOT IMPLEMENTED FOR KEY " + key + " and type=" + getTypeName(pos->second));
    }
}

std::string DataBlock::getFilledItems() const {
    nlohmann::json ret;

    // бежим по массивам и сохраняем данные в json формате
    for (auto & m_intValue : m_intValues) {
        ret[m_intValue.first] = m_intValue.second;
    }

    for (auto & val : m_stringValues) {
        ret[val.first] = val.second;
    }

    for (auto &val : m_doubleValues) {
        ret[val.first] = val.second;
    }

    for (auto &val : m_longValues) {
        ret[val.first] = val.second;
    }

    for (auto &val: m_floatValues) {
        ret[val.first] = val.second;
    }

    for (auto &val: m_boolValues) {
        ret[val.first] = val.second;
    }

    for (auto &val: m_arrayDoubleValues) {
        ret[val.first] = val.second;
    }

    return ret.dump();
}

std::string DataBlock::toJson() const {
    nlohmann::json message;
    auto dataItmes = this->getItems();
    for (const auto& item : dataItmes) {
        switch (this->getItemType(item)) {
            case OR_TYPE_JSON:
                if (!this->getString(item).empty()) {
                    try {
                        message[item] = nlohmann::json::parse(this->getString(item));
                    } catch (const std::exception &ex) {
                        optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, ex.what());
                    }
                }
                break;
            case OR_TYPE_INT:
                message[item] = this->getInt(item);
                break;
            case OR_TYPE_LONG:
                message[item] = this->getLong(item);
                break;
            case OR_TYPE_BOOL:
                message[item] = this->getBool(item);
                break;
            case OR_TYPE_DOUBLE:
                message[item] = this->getDouble(item);
                break;
            case OR_TYPE_FLOAT:
                message[item] = this->getFloat(item);
                break;
            case OR_TYPE_STRING:
                message[item] = this->getString(item);
                break;
            case OR_TYPE_ARRAY:
                message[item] = this->getArray(item);
                break;
            default:
                throw std::runtime_error("DataBlock::toJson(): UNKNOWN TYPE: " + getTypeName(this->getItemType(item)));
        }
    }

    return message.dump();
}

bool DataBlock::parseFromJson(const std::string &content) {
        try {
            nlohmann::json cfg = nlohmann::json::parse(content);
            auto items = this->getItems();

            // запись полей конфига
            for (const auto &key : items) {
                if (cfg.contains(key)) {
                    if (cfg[key].is_number_integer())
                        this->setValueByType(key, optirepair::orutils::getInt(cfg, key, 0));
                    else if (cfg[key].is_number_float())
                        this->setValueByType(key, optirepair::orutils::getDouble(cfg, key, 0.0));
                    else if (cfg[key].is_array()) {
                        if (getItemType(key) != OR_TYPE_ARRAY)
                            this->setValueByType(key, cfg[key].dump());
                        else
                            throw std::runtime_error(__PRETTY_FUNCTION__  + std::string("NOT IMPLEMENTED FOR ARRAY"));
                    } else
                        this->setValueByType(key, cfg.value(key, ""));
                }
            }
        }catch (const std::exception &exception) {
            optirepair::ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Parse config error: ") + exception.what());
            return false;
        }

        return true;
}

std::string DataBlock::getTypeName(OR_DATA_TYPES type) {
    switch (type) {
        case OR_TYPE_STRING:
            return "string";
        case OR_TYPE_JSON:
            return "json";
        case OR_TYPE_INT:
            return "int";
        case OR_TYPE_LONG:
            return "long";
        case OR_TYPE_FLOAT:
            return "float";
        case OR_TYPE_DOUBLE:
            return "double";
        case OR_TYPE_ARRAY:
            return "array";
        case OR_TYPE_BOOL:
            return "bool";
    }
    return "unknown";
}

void DataBlock::clear() {
    m_arrayDoubleValues.clear();
    m_intValues.clear();
    m_doubleValues.clear();
    m_floatValues.clear();
    m_stringValues.clear();
    m_longValues.clear();
    m_boolValues.clear();
}

long long int DataBlock::getBlockTimestamp() {
    return m_timestamp;
}

void DataBlock::setBlockTimestamp(long long timestamp) {
    m_timestamp = timestamp;
}

void DataBlock::pop(const std::string &key) {
    switch (this->getItemType(key)) {
        case OR_TYPE_JSON:
        case OR_TYPE_STRING:
            m_stringValues.erase(key);
            break;
        case OR_TYPE_INT:
            m_intValues.erase(key);
            break;
        case OR_TYPE_LONG:
            m_longValues.erase(key);
            break;
        case OR_TYPE_BOOL:
            m_boolValues.erase(key);
            break;
        case OR_TYPE_DOUBLE:
            m_doubleValues.erase(key);
            break;
        case OR_TYPE_FLOAT:
            m_floatValues.erase(key);
            break;
        case OR_TYPE_ARRAY:
            m_arrayDoubleValues.erase(key);
            break;
        default:
            throw std::runtime_error("DataBlock::pop(): UNKNOWN TYPE: " + getTypeName(this->getItemType(key)));
    }
}
