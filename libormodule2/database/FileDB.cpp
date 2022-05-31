//
// Created by kulakov on 01.07.2021.
//

#include "FileDB.h"

#include <nlohmann/json.hpp>
#include "../ordebug.h"

using namespace optirepair;

void FileDB::init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) {
    std::ignore = moduleType;
    std::ignore = serviceId;
    m_cluster = cluster;
    m_callback = callback;

    nlohmann::json cfg = nlohmann::json::parse(config);
    if (cfg.contains("path")) {
        m_path = cfg["path"];
    }

    uv_timer_init(uv_default_loop(), &m_callbackTimer);
    m_callbackTimer.data = nullptr;
}

void FileDB::updateStatus(const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    std::ofstream out(m_path + "/" + moduleType + "/" + serviceId + ".status", std::ofstream::out | std::ofstream::trunc);
    if (out.is_open()) {
        out << status;
        out.close();
    } else {
        if (!stopPropagation && m_callback) {
            m_callback->databaseGotError(this, m_path + "/" + moduleType + "/" + serviceId + ".status" + " file to write: " + strerror(errno));
        }
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, m_path + "/" + moduleType + "/" + serviceId + ".status" + " file to write: " + strerror(errno));
    }
}

ModuleDescription FileDB::getServiceName(const std::string &serviceID) {
    auto filename = serviceID + ".config";

    // бежим по каталогам типов сервисов и в каждом типе ищем файл с конфигом нужного ID
    DIR *dir;
    struct dirent *ent;
    ModuleDescription ret;
    if ((dir = opendir (m_path.c_str())) != nullptr) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != nullptr) {
            if (ent->d_type == DT_DIR) {
                auto path = m_path + "/" + ent->d_name + "/" + filename;
                auto in = std::ifstream(path);
                if (!in)
                    continue;
                nlohmann::json cfg;
                try {
                    in >> cfg;
                } catch (nlohmann::detail::parse_error &error) {
                    ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Parse config error: ") + error.what());
                    in.close();
                    continue;
                }
                ret.name = cfg["name"];
                ret.description = cfg["description"];
                ret.type = ent->d_name;
                closedir(dir);
                in.close();
                return ret;
            }
        }
        closedir(dir);
    }
    return ret;
}

void FileDB::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    std::string conf_path = m_path + "/" + moduleType + "/" + serviceId + ".config";
    std::ifstream in(conf_path);

    std::string str;

    if (in.is_open()) {
        in.seekg(0, std::ios::end);
        str.reserve(static_cast<unsigned long>(in.tellg()));
        in.seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(in)),
                   std::istreambuf_iterator<char>());
    } else {
        str = "{}";
    }

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Parse config: " + str);

    nlohmann::json cfg;
    try {
        cfg = nlohmann::json::parse(str);
    } catch (nlohmann::detail::parse_error &error) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Parse config error: ") + error.what());
    }

    if (!block->parseFromJson(cfg.dump())) {
        return;
    }

    // отправка конфига
    if (m_callbackTimer.data == nullptr) {
        auto data = new file_callback_data(m_callback);
        data->setBlock(block);
        data->setGuid(serviceId);
        m_callbackTimer.data = data;
        uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getConfig), 0, 0);
    }
}

void FileDB::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    std::ofstream out(m_path + "/" + moduleType + "/" + serviceId + ".second", std::ofstream::out | std::ofstream::trunc);

    if (out.is_open()) {
        out << config;
        out.close();
    } else {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, m_path + "/" + moduleType + "/" + serviceId + ".second"+ " file to write: " + strerror(errno));
    }
}

std::string FileDB::storeData(long long int timestamp, bool useLevel, int level, const std::string &senderType, const std::string &senderId, const std::string &type,
                       DataBlock *data) {
    std::ignore = timestamp;
    std::ignore = useLevel;
    std::ignore = level;
    std::ignore = senderType;
    std::ignore = senderId;
    std::ignore = type;
    std::ignore = data;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void FileDB::storeError(const std::string &data) {
    std::ignore = data;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));

}

void FileDB::timer_callback_getConfig(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<file_callback_data *>(timer->data);

    timer->data = nullptr;

    data->processBlock();
    delete data;
}

void FileDB::timer_callback_getConfigsByType(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<file_callback_data *>(timer->data);

    timer->data = nullptr;

    data->processList();
    delete data;
}

void FileDB::updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock * block) {
    // Читает текущий конфиг
    std::string conf_path = m_path + "/" + moduleType + "/" + serviceId + ".config";
    std::ifstream in(conf_path);

    std::string str;

    if (in.is_open()) {
        in.seekg(0, std::ios::end);
        str.reserve(static_cast<unsigned long>(in.tellg()));
        in.seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(in)),
                   std::istreambuf_iterator<char>());
    } else {
        str = "{}";
    }
    in.close();

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Parse config: " + str);

    nlohmann::json cfg;
    try {
        cfg = nlohmann::json::parse(str);
    } catch (nlohmann::detail::parse_error &error) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Parse config error: ") + error.what());
        return;
    }

    // Добавление полей в конфиг
    nlohmann::json fields = nlohmann::json::parse(block->toJson());
    for (auto field = fields.begin(); field != fields.end(); field++) {
        cfg[field.key()] = field.value();
    }

    // Сохраняет конфиг
    std::ofstream out(conf_path);
    out << cfg;
    out.close();
}

void FileDB::createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) {
    std::ignore = tableName;
    std::ignore = isEventTable;
    std::ignore = senderType;
    std::ignore = senderId;
    std::ignore = block;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void FileDB::getConfigsByType(const std::string &dataType, const DataBlock &block) {
    std::string path = m_path + "/" + dataType;

    std::map<std::string, DataBlock *> ret;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != nullptr) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != nullptr) {
            char *pos;
            if ((pos = strstr(ent->d_name, ".config")) == nullptr)
                continue;
            std::string guid = ent->d_name;
            guid = guid.substr(0, static_cast<unsigned long>(pos - ent->d_name));
            ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Found " + path + "/" + ent->d_name + "; guid=" + guid + "; type=" + dataType);
            std::ifstream in(path + "/" + ent->d_name);

            nlohmann::json cfg;
            try {
                in >> cfg;
            } catch (nlohmann::detail::parse_error &error) {
                ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Parse config error: ") + error.what());
                continue;
            }

            auto bblock = block.createInstance();

            if (!bblock->parseFromJson(cfg.dump())) {
                continue;
            }
            ret[guid] = bblock;
        }
        closedir (dir);

    } else {
        /* could not open directory */
    }

    // отправка конфига
    if (m_callbackTimer.data == nullptr) {
        auto data = new file_callback_data(m_callback);
        data->append(dataType, ret);
        m_callbackTimer.data = data;
        uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getConfigsByType), 0, 0);
    } else {
        auto *data = static_cast<file_callback_data *>(m_callbackTimer.data);
        data->append(dataType, ret);
    }
}

void FileDB::requestData(DataQuery pQuery) {
    std::ignore = pQuery;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

std::vector<std::string> FileDB::getServiceList() {
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

int FileDB::removeData(std::string serviceId, std::string messageType, long long int timestamp) {
    std::ignore = serviceId;
    std::ignore = messageType;
    std::ignore = timestamp;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void FileDB::checkStatus() {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, " NOT IMPLEMENTED");
}
