//
// Created by demo on 18.04.2020.
//

#include "orstatus.h"
#include "ORModule.h"
#include "ordebug.h"
#include "orutils.h"

const unsigned char optirepair::orstatus::E_DC_UNKNOWN;
const unsigned char optirepair::orstatus::E_DC_UNSUPPORTED_PROTOCOL_VERSION;
const unsigned char optirepair::orstatus::E_DC_UNSUPPORTED_PROTOCOL_COMMAND;
const unsigned char optirepair::orstatus::E_DC_UDP_CONNECTION_ERROR;
const unsigned char optirepair::orstatus::E_DC_MEMORY;
const unsigned char optirepair::orstatus::E_DC_PROTOCOL_DATA_LENGTH;
const unsigned char optirepair::orstatus::E_DC_PROTOCOL_DATA_INCORRECT_VALUE;
const unsigned char optirepair::orstatus::E_DC_CHANNEL_ALREADY_BUSY;
const unsigned char optirepair::orstatus::E_DC_NO_ERRORS;

// описание состояний
const unsigned char optirepair::orstatus::CLIENT_ERROR; // произошла ошибка
const unsigned char optirepair::orstatus::CLIENT_NOT_INITIALIZED; // не готов к работе
const unsigned char optirepair::orstatus::CLIENT_START_SUBSCRIPTION; // подписка
const unsigned char optirepair::orstatus::CLIENT_PROCESS; // получение данных по подписке
const unsigned char optirepair::orstatus::CLIENT_ZERO; // произошла ошибка
const unsigned char optirepair::orstatus::CLIENT_CONFIGURING;
const unsigned char optirepair::orstatus::CLIENT_PRIMARY_SETTINGS;
const unsigned char optirepair::orstatus::CLIENT_SECONDARY_SETTINGS;
const unsigned char optirepair::orstatus::CLIENT_READY;
const unsigned char optirepair::orstatus::CLIENT_GET_INFO;

unsigned char optirepair::orstatus::m_lastError = E_DC_NO_ERRORS;
unsigned char optirepair::orstatus::m_currentState = CLIENT_ZERO;
std::string optirepair::orstatus::m_strLastError = std::string();

unsigned char optirepair::orstatus::getLastError() {
    return m_lastError;
}

void optirepair::orstatus::error(const std::string& moduleType, const std::string& serviceId, unsigned char lastError) {
    orstatus::m_lastError = lastError;
    if (lastError != E_DC_NO_ERRORS) {
        updateState(moduleType, serviceId, CLIENT_ERROR);
    } else {
        m_strLastError = "";
    }

    if (lastError != E_DC_NO_ERRORS) {
        ORModule::sendErrorEvent(lastError, whatError());
    }
}

void optirepair::orstatus::error(const std::string& moduleType, const std::string& serviceId, const std::string &str) {
    m_strLastError = str;
    error(moduleType, serviceId, E_DC_UNKNOWN);
}

unsigned char optirepair::orstatus::getCurrentState() {
    return m_currentState;
}

void optirepair::orstatus::updateState(const std::string& moduleType, const std::string& serviceId, unsigned char currentState, bool stopPropagation) {
    if (currentState == orstatus::m_currentState)
        return;

    ordebug::error(__PRETTY_FUNCTION__, __LINE__, " State: " + whatState(currentState) + "; Error: " + whatError());
    orstatus::m_currentState = currentState;
    ORModule::updateStatus(moduleType, serviceId, whatState(currentState), stopPropagation);
}

std::string optirepair::orstatus::whatState(unsigned char state) {
    std::string desc;
    switch (state) {
        case CLIENT_ZERO:
            desc = "Сервис выключен";
            break;
        case CLIENT_NOT_INITIALIZED:
            desc = "Клиент не настроен";
            break;
        case CLIENT_CONFIGURING:
            desc = "Получение конфига";
            break;
        case CLIENT_PRIMARY_SETTINGS:
            desc = "Нет первичных настроек";
            break;
        case CLIENT_SECONDARY_SETTINGS:
            desc = "Нет вторичных настроек";
            break;
        case CLIENT_READY:
            desc = "Все настройки получены";
            break;
        case CLIENT_GET_INFO:
            desc = "Подключение к УСД - получение конфигурации устройства";
            break;
        case CLIENT_START_SUBSCRIPTION:
            desc = "Подключение к УСД - подписка";
            break;
        case CLIENT_PROCESS:
            desc = "Обработка";
            break;
        case CLIENT_ERROR:
            desc = "Произошла ошибка";
            break;
        default:
            desc = "Неизвестное состояние";
            break;
    }

    return desc;
}

std::string optirepair::orstatus::whatError() {
    std::string desc;
    switch (m_lastError) {
        case E_DC_CHANNEL_ALREADY_BUSY:
            desc = "Канал уже занят";
            break;
        case E_DC_NO_ERRORS:
            desc = "Нет ошибок";
            break;
        case E_DC_UDP_CONNECTION_ERROR:
            desc = "Ошибка подключения по UDP";
            break;
        case E_DC_MEMORY:
            desc = "Ошибка работы с памятью";
            break;
        case E_DC_PROTOCOL_DATA_INCORRECT_VALUE:
            desc = "Данные не соответствуют протоколу";
            break;
        case E_DC_PROTOCOL_DATA_LENGTH:
            desc = "Длина данных не соответствует пакету";
            break;
        case E_DC_UNSUPPORTED_PROTOCOL_COMMAND:
            desc = "Неподдерживаемая команда протокола";
            break;
        case E_DC_UNSUPPORTED_PROTOCOL_VERSION:
            desc = "Неподдерживаемая версия протокола";
            break;
        case E_DC_UNKNOWN:
            if (!m_strLastError.empty()) {
                desc = m_strLastError;
            } else {
                desc = "Прочая ошибка";
            }
            break;
        default:
            desc = "Неизвестная ошибка";
    }

    return desc;
}