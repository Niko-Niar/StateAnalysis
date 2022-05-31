//
// Created by demo on 18.04.2020.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_ORSTATUS_H
#define ROOT_OR_SERVER_MONITOR_MODULE_ORSTATUS_H

#include <string>

namespace optirepair {

    /**
     * Реализует отслеживание состояния устройства
     */
    class orstatus {

    public:
        // список ошибок протокола
        static const unsigned char E_DC_UNKNOWN = 0x00;
        static const unsigned char E_DC_UNSUPPORTED_PROTOCOL_VERSION = 0x01;
        static const unsigned char E_DC_UNSUPPORTED_PROTOCOL_COMMAND = 0x02;
        static const unsigned char E_DC_UDP_CONNECTION_ERROR = 0x03;
        static const unsigned char E_DC_MEMORY = 0x04;
        static const unsigned char E_DC_PROTOCOL_DATA_LENGTH = 0x05;
        static const unsigned char E_DC_PROTOCOL_DATA_INCORRECT_VALUE = 0x06;
        static const unsigned char E_DC_CHANNEL_ALREADY_BUSY = 0x07;
        static const unsigned char E_DC_NO_ERRORS = 0xFF;

        // описание состояний
        static const unsigned char CLIENT_ZERO = 0x00; // произошла ошибка
        static const unsigned char CLIENT_NOT_INITIALIZED = 0x01; // не готов к работе
        static const unsigned char CLIENT_CONFIGURING = 0x02;
        static const unsigned char CLIENT_PRIMARY_SETTINGS = 0x03;
        static const unsigned char CLIENT_SECONDARY_SETTINGS = 0x04;
        static const unsigned char CLIENT_READY = 0x05;
        static const unsigned char CLIENT_GET_INFO = 0x06;
        static const unsigned char CLIENT_START_SUBSCRIPTION = 0x07;
        static const unsigned char CLIENT_PROCESS = 0x08; // получение данных по подписке
        static const unsigned char CLIENT_ERROR = 0x09; // произошла ошибка

        /**
         * Получить код последней ошибки
         * @return код ошибки
         */
        static unsigned char getLastError();

        /**
         * Записывает код последней ошибки
         * @param lastError
         */
        static void error(const std::string& moduleType, const std::string& serviceId, unsigned char lastError);

        /**
         * Записывает описание последней ошибки
         * @param str
         */
        static void error(const std::string& moduleType, const std::string& serviceId, const std::string &str);

        /**
         * Возвращает текущее состояние
         * @return код состояния
         */
        static unsigned char getCurrentState();

        /**
         * Записывает текущее состояние
         * @param currentState код состояния
         */
        static void updateState(const std::string& moduleType, const std::string& serviceId, unsigned char currentState, bool stopPropagation = false);

        /**
         * Возвращает описание последней ошибки
         * @return строка описания
         */
        static std::string whatError();

        static std::string whatState(unsigned char state);

    protected:
        static unsigned char m_lastError;
        static unsigned char m_currentState;
        static std::string m_strLastError;
    };
}


#endif //ROOT_OR_SERVER_MONITOR_MODULE_ORSTATUS_H
