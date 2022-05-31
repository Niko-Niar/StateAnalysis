//
// Created by kulakov on 08.07.2021.
//

#include "SystemAPIDB.h"

#include <nlohmann/json.hpp>
#include "../ordebug.h"

using namespace optirepair;

void SystemAPIDB::init(const std::string &cluster, const std::string& moduleType, const std::string& serviceId, const std::string &config, IDatabaseCallback *callback) {
    std::ignore = moduleType;
    std::ignore = serviceId;

    m_cluster = cluster;
    m_callback = callback;

    nlohmann::json cfg = nlohmann::json::parse(config);
    if (cfg.contains("host") && !cfg["host"].empty()) {
        m_host = cfg["host"];
    } else {
        m_host = "http://localhost";
    }

    std::string login = "user";
    if (cfg.contains("login") && !cfg["login"].empty()) {
        login = cfg["login"];
    }

    std::string password = "user";
    if (cfg.contains("password") && !cfg["password"].empty()) {
        password = cfg["password"];
    }

    if (m_curl == nullptr) {
        m_curl = curl_easy_init();
        curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(m_curl, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(m_curl, CURLOPT_MAXREDIRS, 10);
        curl_easy_setopt(m_curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(m_curl, CURLOPT_USERPWD, (login + ":" + password).c_str());
    }

    uv_timer_init(uv_default_loop(), &m_callbackTimer);
    m_callbackTimer.data = nullptr;
}

void SystemAPIDB::updateStatus( const std::string& moduleType, const std::string& serviceId, const std::string &status, bool stopPropagation) {
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback && !stopPropagation) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }

    curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/" + moduleType + "/schemes/" + serviceId).c_str());
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/" + moduleType + "/schemes/" + serviceId);

    // подготовка запроса
    nlohmann::json jdata;
    jdata["status"] = status;

    std::string post_data = jdata.dump();
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, post_data);

    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
    //curl_easy_setopt(m_curl, CURLOPT_PUT, true);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, post_data.c_str() );
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, post_data.length() );

    std::string content;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

    CURLcode res = curl_easy_perform(m_curl);
    if(res != CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);
        lastErrorNum = http_code;
        error_count++;

        if (http_code == 502) {
            return; // игнорируем ошибку деплоя до следующего апдейта
        }

        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        if (m_callback && !stopPropagation) {
            m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        }
        return;
    }

    lastErrorNum = 0; // сброс счетчика ошибок если систем апи хорошо отработало
    error_count = 0;

    if (content == "null")
        return;

    //TODO: {"detail":"Not authenticated"}
    //TODO: {"detail":"Not Found"}

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content + std::to_string(content.size()));
}

ModuleDescription SystemAPIDB::getServiceName(const std::string &serviceId) {
    ModuleDescription ret;
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return ret;
    }
    curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/modules/schemes/" + serviceId).c_str());
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/modules/schemes/" + serviceId);

    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "GET");

    std::string content;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

    CURLcode res = curl_easy_perform(m_curl);
    if(res != CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

        lastErrorNum = http_code;
        error_count++;

        if (http_code == 502) {
            return ret; // игнорируем ошибку деплоя до следующего апдейта
        }

        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        }
        return ret;
    }

    lastErrorNum = 0;
    error_count = 0;

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content);
    nlohmann::json data;
    try {
        data = nlohmann::json::parse(content);

    } catch (const std::exception &exception) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Ошибка разбора: ") + exception.what());
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("Ошибка разбора: ") + exception.what());
        }
        return {};
    }

    if (data.contains("name")) {
        ret.name = data["name"];
        ret.type = data["type"];
        ret.description = data["description"];
        return ret;
    }

    ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Отсутствует имя модуля: ") + content);
    if (m_callback) {
        m_callback->databaseGotError(this, std::string("Отсутствует имя модуля: ") + content);
    }
    return ret;
}

void SystemAPIDB::storeError(const std::string &data) {
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }
    throw std::runtime_error("SystemAPIDB::storeError() NOT IMPLEMENTED!!! data=" + data);
}

void SystemAPIDB::sendConfig(const std::string& moduleType, const std::string& serviceId, const std::string &config) {
    std::ignore = moduleType;
    std::ignore = serviceId;

    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }

    ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "SystemAPIDB::sendConfig() NOT IMPLEMENTED!!! config=" + config);
}

void SystemAPIDB::updateConfigField(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }
    curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/" + moduleType + "/schemes/" + serviceId).c_str());
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/" + moduleType + "/schemes/" + serviceId);

    // подготовка запроса
    nlohmann::json jdata = nlohmann::json::parse(block->getFilledItems());

    std::string post_data = jdata.dump();
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, post_data);

    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
    //curl_easy_setopt(m_curl, CURLOPT_PUT, true);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, post_data.c_str() );
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, post_data.length() );

    std::string content;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

    CURLcode res = curl_easy_perform(m_curl);
    if(res != CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

        lastErrorNum = http_code;
        error_count++;

        if (http_code == 502) {
            return; // игнорируем ошибку деплоя до следующего апдейта
        }

        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        }
        return;
    }

    error_count = 0;

    if (content == "null")
        return;

    //TODO: {"detail":"Not authenticated"}
    //TODO: {"detail":"Not Found"}

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content + std::to_string(content.size()));
}

void SystemAPIDB::getConfig(const std::string& moduleType, const std::string& serviceId, DataBlock *block) {
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }
    curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/" + moduleType + "/schemes/" + serviceId).c_str());
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/" + moduleType + "/schemes/" + serviceId);

    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "GET");

    std::string content;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

    CURLcode res = curl_easy_perform(m_curl);
    if(res != CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

        lastErrorNum = http_code;
        error_count++;

        if (http_code == 502) {
            return; // игнорируем ошибку деплоя до следующего апдейта
        }

        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        }
        return;
    }

    error_count = 0;

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content);
    nlohmann::json data;
    try {
        data = nlohmann::json::parse(content);

    } catch (const std::exception &exception) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Ошибка разбора: ") + exception.what());
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("Ошибка разбора: ") + exception.what());
        }
        return;
    }

    // parse config
    auto items = block->getItems();
    for (const auto& key : items) {
        if (data.contains(key)) {
            if (data[key].is_number_integer())
                block->setValueByType(key, data.value(key, 0));
            else if (data[key].is_number_float())
                block->setValueByType(key, data.value(key, 0.0));
            else if (data[key].is_boolean()) {
                block->setValue(key, data.value(key, false));
            } else
                block->setValue(key, data.value(key, ""));
        } else {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Key " + key + " not found in config!!!");
        }
    }

    // отправка конфига
    if (m_callbackTimer.data == nullptr) {
        auto cdata = new systemapi_callback_data(m_callback);
        cdata->setBlock(block);
        cdata->setGuid(serviceId);
        m_callbackTimer.data = cdata;
        uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getConfig), 0, 0);
    }
}

std::string SystemAPIDB::storeData(long long int timestamp, bool useLevel, int level,
                                   const std::string &senderType, const std::string &senderId, const std::string &type, DataBlock *data) {
    std::ignore = level;
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return {};
    }

    std::string realType = type;
    std::replace(realType.begin(), realType.end(), '_', '-');

    std::string content;
    // цикл попыток, нужен для повторного запуска записи данных если таблицы не найдены
    for(auto attempt = 0; attempt < 2; attempt++) {
        curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/" + senderType + "/" + senderId + "/events/" + realType).c_str());
        ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/" + senderType + "/" + senderId + "/events/" + realType);

        // подготовка запроса
        nlohmann::json jdata;
        jdata["timestamp"] = timestamp;
        auto items = data->getItems();
        for (const auto& key : items) {
            switch (data->getItemType(key)) {
                case OR_TYPE_STRING:
                {
                    jdata[key] = data->getString(key);
                    break;
                }
                case OR_TYPE_JSON: {
                    jdata[key] = nlohmann::json::parse(data->getString(key));
                    break;
                }
                case OR_TYPE_INT: {
                    jdata[key] = data->getInt(key);
                    break;
                }
                case OR_TYPE_LONG: {
                    jdata[key] = data->getLong(key);
                    break;
                }
                case OR_TYPE_FLOAT: {
                    jdata[key] = data->getFloat(key);
                    break;
                }
                case OR_TYPE_DOUBLE: {
                    jdata[key] = data->getDouble(key);
                    break;
                }
                case OR_TYPE_ARRAY: {
                    jdata[key] = data->getArray(key);
                    break;
                }
                case OR_TYPE_BOOL: {
                    jdata[key] = data->getBool(key);
                    break;
                }
                default: {
                    throw std::runtime_error(__PRETTY_FUNCTION__ + std::string(" NOT IMPLEMENTED FOR TYPE") +
                    std::to_string(data->getItemType(key)));
                }
            }
        }
        std::string post_data = jdata.dump();

        curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, post_data.c_str() );
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, post_data.length() );

        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

        CURLcode res = curl_easy_perform(m_curl);
        if(res != CURLE_OK) {
            long http_code = 0;
            curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

            lastErrorNum = http_code;
            error_count++;

            if (http_code == 502) {
                return {}; // игнорируем ошибку деплоя до следующего апдейта
            }

            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content-size: " + std::to_string(content.size()));
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "url:" + m_host + "/api/v1/" + senderType + "/" + senderId + "/events/" + realType + " content:" + post_data);

            if (attempt == 0) {
                // если первая попытка не удалась, то создаем таблицы и пробуем снова
                this->createTableIfNotExists(type, useLevel, senderType, senderId, data);
                continue;
            }
            if (m_callback) {
                m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
            }
            return {};
        }

        error_count = 0;

        if (content == "null")
            return {};

        // если все ок, но сервер что-то вернул, то выходим из цикла попыток
        break;
    }
    //TODO: {"detail":"Not authenticated"}
    //TODO: {"detail":"Not Found"}

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Content size = " + std::to_string(content.size()));

    return {};
}

size_t SystemAPIDB::write_data(char *ptr, size_t size, size_t nmemb, std::string* data)
{
    if (data)
    {
        data->append(ptr, size*nmemb);
        return size*nmemb;
    }
    else return 0;  // будет ошибка
}

void SystemAPIDB::timer_callback_getConfig(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<systemapi_callback_data *>(timer->data);

    timer->data = nullptr;

    data->processConfig();
    delete data;
}

void SystemAPIDB::timer_callback_getConfigsByType(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<systemapi_callback_data *>(timer->data);

    timer->data = nullptr;

    data->processList();
    delete data;
}

void SystemAPIDB::createTableIfNotExists(const std::string &tableName, bool isEventTable, const std::string &senderType, const std::string &senderId, DataBlock *block) {
    std::ignore = tableName;
    std::ignore = block;
    std::ignore = isEventTable;
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }

    curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/" + senderType + "/" + senderId + "/events").c_str());
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/" + senderType + "/" + senderId + "/events");

    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");

    std::string content;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

    CURLcode res = curl_easy_perform(m_curl);
    if(res != CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

        lastErrorNum = http_code;
        error_count++;

        if (http_code == 502) {
            return; // игнорируем ошибку деплоя до следующего апдейта
        }

        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        }
        return;
    }

    error_count = 0;

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content);
}

void SystemAPIDB::getConfigsByType(const std::string &dataType, const DataBlock &block) {
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }

    //throw std::runtime_error(__PRETTY_FUNCTION__ + std::string(" NOT IMPLEMENTED!!! data=") + dataType);

    //TODO: отправить полученные конфиги
    curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/" + dataType + "/").c_str());
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/" + dataType + "/");

    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "GET");

    std::string content;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

    CURLcode res = curl_easy_perform(m_curl);
    if(res != CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

        lastErrorNum = http_code;
        error_count++;

        if (http_code == 502) {
            return; // игнорируем ошибку деплоя до следующего апдейта
        }

        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        }
        return;
    }

    error_count = 0;

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content);
    nlohmann::json configs;
    try {
        configs = nlohmann::json::parse(content);

    } catch (const std::exception &exception) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Ошибка разбора: ") + exception.what());
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("Ошибка разбора: ") + exception.what());
        }
        return;
    }

    // parse config
    std::map<std::string, DataBlock *> ret;
    for (auto config : configs) {
        // запрос конфига по guid
        auto bblock = block.createInstance();

        curl_easy_setopt(m_curl, CURLOPT_URL, (m_host + "/api/v1/" + dataType + "/schemes/" + config["guid"].get<std::string>()).c_str());
        ordebug::info(__PRETTY_FUNCTION__ , __LINE__, m_host + "/api/v1/" + dataType + "/schemes/" + config["guid"].get<std::string>());

        curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "GET");

        content = "";
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

        res = curl_easy_perform(m_curl);
        if(res != CURLE_OK) {
            long http_code = 0;
            curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

            lastErrorNum = http_code;
            error_count++;

            if (http_code == 502) {
                return; // игнорируем ошибку деплоя до следующего апдейта
            }

            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
            if (m_callback) {
                m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
            }
            continue;
        }

        error_count = 0;

        ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content);
        nlohmann::json data;
        try {
            data = nlohmann::json::parse(content);

        } catch (const std::exception &exception) {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Ошибка разбора: ") + exception.what());
            if (m_callback) {
                m_callback->databaseGotError(this, std::string("Ошибка разбора: ") + exception.what());
            }
            continue;
        }

        // parse config
        if (!bblock->parseFromJson(data.dump())) {
            continue;
        }
        ret[config["guid"]] = bblock;
    }

    // отправка конфига
    if (m_callbackTimer.data == nullptr) {
        auto data = new systemapi_callback_data(m_callback);
        data->append(dataType, ret);
        m_callbackTimer.data = data;
        uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getConfigsByType), 0, 0);
    } else {
        auto *data = static_cast<systemapi_callback_data *>(m_callbackTimer.data);
        data->append(dataType, ret);
    }
}

void SystemAPIDB::requestData(DataQuery pQuery) {
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return;
    }

    std::string realType = pQuery.getType();
    std::replace(realType.begin(), realType.end(), '_', '-');

    auto url = m_host + "/api/v1/" + pQuery.getModuleType() + "/" + pQuery.getModuleId() + "/events/" + realType + "?";
    if (pQuery.getTimestamp() > 0) {
        url += "start_date=" + std::to_string(pQuery.getTimestamp()) + "&end_date=" + std::to_string(pQuery.getTimestamp());
    } else {
        std::string order = R"([{"name":"OrderByField","order":-1,"field":"timestamp"}])";
        char *ordr = curl_easy_escape(m_curl, order.c_str(), static_cast<int>(strlen(order.c_str())));
        url += "order_by=" + std::string(ordr);
        curl_free(ordr);
    }

    if (!pQuery.getFilter().empty()) {
        auto condData = nlohmann::json::parse(pQuery.getFilter());
        nlohmann::json filter;
        for (auto &item : condData.items()) {
            nlohmann::json filterItem;
            filterItem["name"] = "FieldValueFilter";
            filterItem["field"] = item.key();
            filterItem["value"] = item.value();
            filterItem["op"] = "eq";
            filter.push_back(filterItem);
        }

        auto txtFilter = filter.dump();
        ordebug::info(__PRETTY_FUNCTION__ , __LINE__, txtFilter);

        char * excFilters = curl_easy_escape(m_curl, txtFilter.c_str(), static_cast<int>(strlen(txtFilter.c_str())));
        url += "&filters=" + std::string(excFilters);
        curl_free(excFilters);
    }

    url += "&limit=1";

    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, url);

    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "GET");

    std::string content;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA,     &content);

    CURLcode res = curl_easy_perform(m_curl);
    if(res != CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo (m_curl, CURLINFO_RESPONSE_CODE, &http_code);

        lastErrorNum = http_code;
        error_count++;

        if (http_code == 502) {
            return; // игнорируем ошибку деплоя до следующего апдейта
        }

        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("CURL error: ") + curl_easy_strerror(res) + "; code=" + std::to_string(http_code) + "; content: " + content);
        }
        return;
    }

    error_count = 0;

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, content);
    nlohmann::json data;
    try {
        data = nlohmann::json::parse(content);
        if (!data.is_array()) {
            throw std::logic_error("content not array");
        }
    } catch (const std::exception &exception) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, std::string("Ошибка разбора: ") + exception.what());
        if (m_callback) {
            m_callback->databaseGotError(this, std::string("Ошибка разбора: ") + exception.what());
        }
        return;
    }

    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, data.dump());

    // если в результате запроса не данных, то ничего не делаем
    if (data.at(0).empty())
        return;

    for (size_t i = 0; i < data.at(0).size(); i++) {
        // parse item
        auto block = pQuery.getBlock()->createInstance();
        auto items = block->getItems();
        for (size_t j = 0; j < data.at(0).at(i).size(); j++) {
            auto name = data.at(1).at(j).at(0).get<std::string>();
            auto type = data.at(1).at(j).at(1).get<std::string>();
            if (name == "timestamp") {
                block->setBlockTimestamp(data.at(0).at(i).at(j).get<int>());
                continue;
            }
            if (std::find(items.begin(), items.end(), name) == items.end()) { // проверка того, что имя параметра есть в блоке
                ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Key " + name + " not found in block structure!!!");
                continue;
            }
            if (type == "UInt64") {
                block->setValueByType(name, data.at(0).at(i).at(j).get<long>());
            } else if (type == "UInt32" || type == "UInt8" || type == "UInt16") {
                block->setValueByType(name, data.at(0).at(i).at(j).get<int>());
            } else if (type == "Float32") {
                block->setValueByType(name, data.at(0).at(i).at(j).get<double>());
            } else {
                ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Unknown type " + type + "; use string");
                block->setValueByType(name, data.at(0).at(i).at(j).get<std::string>());
            }
        }

        if (block->getBlockTimestamp() > 0) {
            m_callback->databaseGotData(pQuery, block);
        } else {
            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Got data without timestamp");
        }

//        // отправка данных блока
//        if (m_callbackTimer.data == nullptr) {
//            auto cdata = new systemapi_callback_data(m_callback);
//            cdata->setBlock(block);
//            cdata->setQuery(pQuery);
//            cdata->setGuid(pQuery.getModuleId());
//            m_callbackTimer.data = cdata;
//            uv_timer_start(&m_callbackTimer, reinterpret_cast<uv_timer_cb>(&timer_callback_getData), 0, 0);
//        } else {
//            ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "send more than one value NOT IMPLEMENTED!!!");
//        }
    }
}

std::vector<std::string> SystemAPIDB::getServiceList() {
    if (m_curl == nullptr) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "curl not initialised!");
        if (m_callback) {
            m_callback->databaseGotError(this, "curl not initialised!");
        }
        return {};
    }

    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

int SystemAPIDB::removeData(std::string serviceId, std::string messageType, long long int timestamp) {
    std::ignore = serviceId;
    std::ignore = messageType;
    std::ignore = timestamp;
    throw std::runtime_error(__PRETTY_FUNCTION__  + std::string(" NOT IMPLEMENTED"));
}

void SystemAPIDB::timer_callback_getData(uv_timer_t *timer) {
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Run timer callback");
    auto data = static_cast<systemapi_callback_data *>(timer->data);

    timer->data = nullptr;

    data->processQueryData();
    delete data;
}

void SystemAPIDB::checkStatus() {
    if (error_count > 0) {
        ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "Connection error code: " + std::to_string(lastErrorNum));
    }
    error_count = 0;
    lastErrorNum = 0;
}
