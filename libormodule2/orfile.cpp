//
// Created by demo on 17.04.2020.
//

#include "orfile.h"
#include "ordebug.h"
#include "ORModule.h"
#include "datatypes/FileData.h"
#include "orutils.h"

#include <nlohmann/json.hpp>
#include <ctime>
#include <utility>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <iomanip>
using namespace nlohmann;

std::string optirepair::orfile::dir_name;
std::string optirepair::orfile::result_source_filename;
std::string optirepair::orfile::result_filter_filename;
std::string optirepair::orfile::result_working_filename;
std::string optirepair::orfile::result_export_filename;
std::string optirepair::orfile::prefix_working_filename;
std::string optirepair::orfile::prefix_export_filename;
std::string optirepair::orfile::prefix_filter_filename;
bool optirepair::orfile::source_record;
bool optirepair::orfile::filter_record;
bool optirepair::orfile::working_record;
bool optirepair::orfile::export_record;
time_t optirepair::orfile::source_start_time;
time_t optirepair::orfile::filter_start_time;
time_t optirepair::orfile::working_start_time;
time_t optirepair::orfile::export_start_time;
int optirepair::orfile::source_duration;
int optirepair::orfile::filter_duration;
int optirepair::orfile::working_duration;
int optirepair::orfile::export_duration;
std::ofstream optirepair::orfile::source_file;
std::ofstream optirepair::orfile::filter_file;
std::ofstream optirepair::orfile::working_file;
std::ofstream optirepair::orfile::export_file;

#define UNUSED(x) (void)(x)


void optirepair::orfile::write_working_file(std::vector<double> &data, const std::string& filename, time_t timestamp) {
    if (working_record && !data.empty()) {
        if (!working_file.is_open()) {
            result_working_filename = dir_name + prefix_working_filename + filename;
            working_file.open(result_working_filename);
            working_file << fixed;
            ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Открытие файла на запись по тахометру: " + result_working_filename);
        }

        if (working_file.is_open()) {
            for (double i : data) {
                working_file << i << " ";
            }
        }
    }
}

void optirepair::orfile::write_export_file(std::vector<double> &data, const std::string& filename, time_t timestamp) {
    if (export_record && !data.empty()) {
        if (!export_file.is_open()) {
            export_start_time = timestamp;
            result_export_filename = dir_name + prefix_export_filename + filename;
            export_file.open(result_export_filename);
            export_file << fixed;
            ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Открытие файла для записи на экспорт: " + result_export_filename);
        }

        if (timestamp > export_duration + export_start_time) {
            export_record = false;
            export_file << std::endl;
            export_file.close();

            FilePathData fdata(result_export_filename);
            ORModule::sendActionResponse(orutils::timeInMilliseconds(), "export-result", ORModule::getServiceId(), &fdata);
            ORModule::sendNotification(orutils::timeInMilliseconds(), 0, ORModule::getServiceId(), "export-result", &fdata);
            return;
        }

        if (export_file.is_open()) {
            for (double i : data) {
                export_file << i << " ";
            }
        }
    }
}

void optirepair::orfile::write_source_file(std::vector<double> &data, const std::string& filename, time_t timestamp) {
    if (source_record && !data.empty()) {
        if (!source_file.is_open()) {
            source_start_time = timestamp;
            result_source_filename = dir_name + filename;
            source_file.open(result_source_filename);
            source_file << fixed;
            ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Открытие файла для записи сырого сигнала: " + result_source_filename);
        }

        if (timestamp > source_duration + source_start_time) {
            source_record = false;
            source_file << std::endl;
            source_file.close();

            FileData fdata("file", result_source_filename);
            ORModule::sendActionResponse(orutils::timeInMilliseconds(), "start_record", ORModule::getServiceId(), &fdata);
            //ORModule::storeEvent(orutils::timeInMilliseconds(), "action-result", 0, "", ORModule::getServiceId(), "file", &fdata);
            return;
        }

        if (source_file.is_open()) {
            for (double i : data) {
                source_file << i << " ";
            }
        }
    }
}

void optirepair::orfile::write_filter_file(std::vector<double> &data, const std::string& filename, time_t timestamp) {
    if (filter_record && !data.empty()) {
        if (!filter_file.is_open()) {
            filter_start_time = timestamp;
            result_filter_filename = dir_name + prefix_filter_filename + filename;
            filter_file.open(result_filter_filename);
            filter_file << fixed;
            ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Открытие файла для записи фильтрованного сигнала: " + result_filter_filename);
        }

        if (timestamp > filter_duration + filter_start_time) {
            filter_record = false;
            filter_file << std::endl;
            filter_file.close();

            FileData fdata("file", result_filter_filename);
            fdata.update("file", result_filter_filename, "after_filter");
            ORModule::sendActionResponse(orutils::timeInMilliseconds(), "start_record", ORModule::getServiceId(), &fdata);
            //ORModule::storeEvent(orutils::timeInMilliseconds(), "action-result", 0, "", ORModule::getServiceId(), "file", &fdata);
            return;
        }

        if (filter_file.is_open()) {
            for (double i : data) {
                filter_file << i << " ";
            }
        }
    }
}

void optirepair::orfile::appendData(std::vector<double> &data, const std::string& filename, long long timestamp) {
    write_working_file(data, filename, timestamp / 1000);
    write_export_file(data, filename, timestamp / 1000);
    write_source_file(data, filename, timestamp / 1000);
}

void optirepair::orfile::appendFilterData(std::vector<double> &data, const std::string& filename, long long timestamp) {
    write_filter_file(data, filename, timestamp / 1000);
}

void optirepair::orfile::appendData(std::vector<ulong> &data, const std::string& filename) {
    UNUSED(data);
    UNUSED(filename);
    ordebug::error(__PRETTY_FUNCTION__ , __LINE__, "orfile::appendData() NOT IMPLEMENTED!!!!");
}

void optirepair::orfile::init(const std::string &configPath) {
    std::ifstream in(configPath);

    dir_name = "/tmp/service/";
    if (in.is_open()) {
        try {
            json config;
            in >> config;

            if (config["storage"]["filesystem"]["path"] != nullptr) {
                dir_name = std::string(config["storage"]["filesystem"]["path"]) + std::string("/");
            }
        } catch (std::exception& ex) {
            ordebug::error(__PRETTY_FUNCTION__, __LINE__, ex.what());
        }
    }

    dir_name += ORModule::getServiceId() + "/";
    if (0 == mkdir(dir_name.data(), 0777))
        ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Создание директории..");

    ordebug::info(__PRETTY_FUNCTION__, __LINE__, dir_name);
}

void optirepair::orfile::setRecord(int duration) {
    if (!source_record) {
        source_record = true;
        source_duration = duration;
    }

    if (!filter_record) {
        prefix_filter_filename = "after_filter_";
        filter_record = true;
        filter_duration = duration;
    }
}

std::string &optirepair::orfile::getResultFileName() {
    return result_source_filename;
}

void optirepair::orfile::setWorkingRecord(bool record, const std::string& prefix) {
    prefix_working_filename = prefix;
    setWorkingRecord(record);
}

void optirepair::orfile::setWorkingRecord(bool record) {
    if (!record) {
        working_record = false;
	    working_file << endl;
        working_file.close();
    } else {
        working_record = true;
    }
}

bool optirepair::orfile::getTahoRecordStatus() {
    return working_record;
}

string optirepair::orfile::getTahoFileName() {
    return result_working_filename;
}

void optirepair::orfile::stopTahoRecord() {
    if (!working_record)
        return;

    working_record = false;
    working_file << endl;
    working_file.close();
    if (remove(result_working_filename.data())) {
        ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Файл удален по отмене записи: " + result_working_filename);
    }
}

bool optirepair::orfile::write(vector<double> &data, const string &filename, const string &type) {
    ofstream out;
    std::string path = dir_name + filename;
    out.open(path);

    if (!out.is_open()) {
        return false;
    }
    // Запись в файл
    for (double i : data) {
        out << i << " ";
    }
    out.close();

    // Уведомление о записи в файл
    FilePathData fdata(path);
    ORModule::sendNotification(orutils::timeInMilliseconds(), 0, ORModule::getServiceId(), type, &fdata);

    return true;
}

void optirepair::orfile::setExportRecord(int duration) {
    if (!export_record) {
        prefix_export_filename = "export_";
        export_record = true;
        export_duration = duration;
    }
}
