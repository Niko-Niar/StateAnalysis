//
// Created by demo on 17.04.2020.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_ORFILE_H
#define ROOT_OR_SERVER_MONITOR_MODULE_ORFILE_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;
namespace optirepair {

    /**
     * Отвечает за запись в файл сигналов
     */
    class orfile {

    public:
        /**
         * Получение настроек из файла конфигурации
         */
        static void init(const std::string &configPath);

        static std::string & getResultFileName();

        /**
         * Сохраняет сырые данные в файл
         * @param data - данные
         * @param filename - имя файла
         */
        static void appendData(std::vector<double>& data, const std::string& filename, long long timestamp);

        /**
         * Сохраняет сырые данные в файл
         * @param data - данные
         * @param filename - имя файла
         */
        static void appendData(std::vector<ulong>& data, const std::string& filename);

        /**
         * Устанавливает флаг записи
         * @param record true - вести запись, false - не записывать
         */
        static void setRecord(int duration);

        static void setExportRecord(int duration);

        /**
         * Запись в файл когда работает тахометр
         * @param record
         * @param prefix
         */
        static void setWorkingRecord(bool record, const std::string& prefix);

        static void appendFilterData(std::vector<double> &data, const std::string& filename, long long timestamp);

        static bool getTahoRecordStatus();

        static string getTahoFileName();

        static void stopTahoRecord();

        static bool write(vector<double> &data, const string &filename, const string &type);

    private:
        static void setWorkingRecord(bool record);

        static void write_export_file(std::vector<double> &data, const std::string& filename, time_t timestamp);
        static void write_working_file(std::vector<double> &data, const std::string& filename, time_t timestamp);
        static void write_source_file(std::vector<double> &data, const std::string& filename, time_t timestamp);
        static void write_filter_file(std::vector<double> &data, const std::string& filename, time_t timestamp);

        /**
         * Директория для сохранения файлов
         */
        static std::string dir_name;

        /**
         * Итоговое название файла с сырыми данными
         */
        static std::string result_source_filename;

        /**
         * Итоговое название файла с фильтрованными данными
         */
        static std::string result_filter_filename;

        /**
         * Итоговое название файла с данными во время работы узла
         */
        static std::string result_working_filename;

        /**
         * Итоговое название файла с сырыми данными на экспорт
         */
        static std::string result_export_filename;

        /**
         * Префикс к названию файла сигнала при работающем узле
         */
        static std::string prefix_working_filename;

        /**
         * Префикс к названию файла сырого сигнала на экспорт
         */
        static std::string prefix_export_filename;

        /**
         * Префикс к названию файла c фильтрованным сигналом
         */
        static std::string prefix_filter_filename;

        /**
         * Время начала записи сырого сигнала
         */
        static time_t source_start_time;

        /**
         * Время начала записи фильтрованного сигнала
         */
        static time_t filter_start_time;

        /**
         * Время начала записи сигнала во время работы узла
         */
        static time_t working_start_time;

        /**
         * Время начала записи файла на экспорт
         */
        static time_t export_start_time;

        /**
         * Длительность записи сырого сигнала
         */
        static int source_duration;

        /**
         * Длительность записи фильтрованного сигнала
         */
        static int filter_duration;

        /**
         * Длительность записи сигнала во время работы узла
         */
        static int working_duration;

        /**
         * Длительность записи файла на экспорт
         */
        static int export_duration;

        /**
         * Записывать сырой сигнал
         */
        static bool source_record;

        /**
         * Записывать сигнал после фильтра
         */
        static bool filter_record;

        /**
         * Записывать сигнал в файл для сигнала во время работы узла
         */
        static bool working_record;

        /**
         * Записывать сигнал в файл на экспорт
         */
        static bool export_record;

        /**
         * Файл для записи сырых данных
         */
        static std::ofstream source_file;

        /**
         * Файл для записи фильтрованных данных
         */
        static std::ofstream filter_file;

        /**
         * Файл для записи данных в рабочем режиме узла
         */
        static std::ofstream working_file;

        /**
         * Файл для записи сырых данных на экспорт
         */
        static std::ofstream export_file;
    };
}

#endif //ROOT_OR_SERVER_MONITOR_MODULE_ORFILE_H
