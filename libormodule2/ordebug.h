//
// Created by kulakov on 30.03.2020.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_ORDEBUG_H
#define ROOT_OR_SERVER_MONITOR_MODULE_ORDEBUG_H

#include <string>
#include <fstream>

namespace optirepair {

    class ordebug {
    public:
        /**
         * Печать отладочного вывода
         * @param file файл
         * @param line строка
         * @param message текст сообщения
         */
        static void info(const std::string &file, int line, const std::string &message);
        static void info(const std::string &file, int line, int message);

        /**
         * Печать предупреждения
         * @param file файл
         * @param line строка
         * @param message текст сообщения
         */
        static void warning(const std::string &file, int line, const std::string &message);

        /**
         * Печать ошибки без проверок
         * @param file файл
         * @param line строка
         * @param message текст сообщения
         * @return строку ошибки
         */
        static std::string error(const std::string &file, int line, const std::string &message);

        /**
         * Включение/выключение записи в файл
         * @param serviceId ИД сервиса
         * @param isStoreToLogFile статус (вкл/выкл)
         */
        static void storeToFile(const std::string &serviceId, bool isStoreToLogFile);

    private:
        static ordebug *getInstance();

        ordebug();

        static ordebug *m_instance;

        /**
         * Статус отладочного вывода на консоль (вкл/выкл)
         */
        bool m_isEnabled = false;

        /**
         * Статус отладочного вывода в файл (вкл/выкл)
         */
        bool m_isStoreToLogFile = false;

        /**
         * Дескриптор записи в файл
         */
        std::ofstream m_logfile{};

        /**
         * Имя файла для переоткрытия
         */
        std::string m_logFileName{};


        ino_t m_inode = 0;
    };
}

#endif //ROOT_OR_SERVER_MONITOR_MODULE_ORDEBUG_H
