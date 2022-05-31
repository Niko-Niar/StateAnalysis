#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <sstream>

#include "ordebug.h"

using optirepair::ordebug;

ordebug *ordebug::m_instance;

ordebug::ordebug() {
    auto val = std::getenv("ORMODULE_DEBUG");
    this->m_isEnabled = val != nullptr;
}

ordebug *ordebug::getInstance() {
    if (!ordebug::m_instance)
        ordebug::m_instance = new ordebug();

    return ordebug::m_instance;
}

void ordebug::info(const std::string &file, int line, const std::string &message) {
    time_t seconds = time(nullptr);
    tm* timeinfo = localtime(&seconds);
    std::string strtime = asctime(timeinfo);
    if (!strtime.empty()) {
        strtime[strtime.size() - 1] = ']';
        strtime = "[" + strtime + " ";
    }

    if (ordebug::getInstance()->m_isEnabled) {
        std::cout << strtime << file << " " << line << ": " << message << std::endl;
    }
}

void optirepair::ordebug::warning(const std::string &file, int line, const std::string &message) {
    time_t seconds = time(nullptr);
    tm* timeinfo = localtime(&seconds);
    std::string strtime = asctime(timeinfo);
    if (!strtime.empty()) {
        strtime[strtime.size() - 1] = ']';
        strtime = "[" + strtime + " ";
    }
    if (ordebug::getInstance()->m_isEnabled) {
        std::cerr << strtime << file << " " << line << ": " << message << std::endl;
    }
    if (ordebug::getInstance()->m_logfile.is_open()) {
        struct stat mystat{};
        if (stat(ordebug::getInstance()->m_logFileName.c_str(), &mystat)!=0 ||
            mystat.st_ino != ordebug::getInstance()->m_inode) {
            ordebug::getInstance()->m_logfile.close();
            ordebug::getInstance()->m_logfile.open(ordebug::getInstance()->m_logFileName, std::ios_base::out & std::ios_base::app);
            ordebug::getInstance()->m_inode = mystat.st_ino;
        }
        ordebug::getInstance()->m_logfile << "WARNING: " << strtime << file << " " << line << ": " << message << std::endl;
        ordebug::getInstance()->m_logfile.flush();
    }
}

std::string optirepair::ordebug::error(const std::string &file, int line, const std::string &message) {
    time_t seconds = time(nullptr);
    tm* timeinfo = localtime(&seconds);
    std::string strtime = asctime(timeinfo);
    if (!strtime.empty()) {
        strtime[strtime.size() - 1] = ']';
        strtime = "[" + strtime + " ";
    }
    std::ostringstream s_stream;
    s_stream << strtime << file << " " << line << ": " << message << std::endl;
    std::cerr << s_stream.str();
    if (ordebug::getInstance()->m_logfile.is_open()) {
        struct stat mystat{};
        if (stat(ordebug::getInstance()->m_logFileName.c_str(), &mystat)!=0 ||
            mystat.st_ino != ordebug::getInstance()->m_inode) {
            ordebug::getInstance()->m_logfile.close();
            ordebug::getInstance()->m_logfile.open(ordebug::getInstance()->m_logFileName, std::ios_base::out & std::ios_base::app);
            ordebug::getInstance()->m_inode = mystat.st_ino;
        }
        ordebug::getInstance()->m_logfile << "ERROR: " << strtime << file << " " << line << ": " << message << std::endl;
        ordebug::getInstance()->m_logfile.flush();
    }
    return s_stream.str();
}

void optirepair::ordebug::info(const std::string &file, int line, int message) {
    ordebug::info(file, line, std::to_string(message));
}

void optirepair::ordebug::storeToFile(const std::string &serviceId, bool isStoreToLogFile) {
    if (ordebug::getInstance()->m_isStoreToLogFile) {
        ordebug::getInstance()->m_logfile.close();
        ordebug::getInstance()->m_logFileName.clear();
    }

    ordebug::getInstance()->m_isStoreToLogFile = isStoreToLogFile;
    if (isStoreToLogFile) {
        ordebug::getInstance()->m_logFileName = "/var/log/optirepair/" + serviceId + ".log";
        struct stat linfo{};

        if( stat( "/var/log/optirepair/", &linfo ) != 0 ) {
            printf("cannot access %s\n", "/var/log/optirepair/");
            mkdir("/var/log/optirepair/", 0755);
        } else if( linfo.st_mode & S_IFDIR ) { // S_ISDIR() doesn't exist on my windows
            ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "/var/log/optirepair/ exists");
        } else {
            error(__PRETTY_FUNCTION__, __LINE__, "/var/log/optirepair/ is no directory");
            return;
        }
        ordebug::getInstance()->m_logfile.open(ordebug::getInstance()->m_logFileName, std::ios_base::out & std::ios_base::app);
    }
}
