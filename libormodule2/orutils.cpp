//
// Created by demo on 20.10.2020.
//

#include "orutils.h"

#include <sstream>
#include <cmath>
#include <iomanip>
#include <sys/time.h>

std::string optirepair::orutils::str_round(double x, int y){
    std::ostringstream s;
    // флаг fixed используется т.к. если задано знаков меньше чем в числе то идет переход к математической записи
    s << std::setprecision(y) << setiosflags(std::ios::fixed) << floor(x*pow(10,y))/(1.0*pow(10,y));
    return s.str();
}
std::string optirepair::orutils::str_round(float x, int y){
    std::ostringstream s;
    // флаг fixed используется т.к. если задано знаков меньше чем в числе то идет переход к математической записи
    s << std::setprecision(y) << setiosflags(std::ios::fixed) << floor(x*pow(10,y))/(1.0*pow(10,y));
    return s.str();
}

double optirepair::orutils::getDouble(nlohmann::json jsn, const std::string& item, double def) {
    try {
        if (!(jsn[item].is_number() || jsn[item].is_string()) || jsn[item].is_null())
            return def;
    } catch (const std::exception &exception) {
        return def;
    }

    if (jsn[item].is_number())
        return jsn[item];

    auto str = jsn[item].get<std::string>();
    return orutils::parseStringWithDouble(str);
}

bool optirepair::orutils::iequals(const std::string& a, const std::string& b) {
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

int optirepair::orutils::getInt(nlohmann::json jsn, const std::string& item, int def) {
    try {
        if (!(jsn[item].is_number() || jsn[item].is_string()) || jsn[item].is_null())
            return def;
    } catch (const std::exception &exception) {
        return def;
    }

    if (jsn[item].is_number())
        return jsn[item];


    auto str = jsn[item].get<std::string>();
    return std::stoi(str);
}

std::string optirepair::orutils::getString(nlohmann::json jsn, const std::string& item) {
    try {
        if (!(jsn[item].is_number() || jsn[item].is_string()) || jsn[item].is_null())
            return {};
    } catch (const std::exception &exception) {
        return {};
    }
    if (jsn[item].is_number_integer()) {
        int i = jsn[item];
        return std::to_string(i);
    }
    if (jsn[item].is_number_float()){
        std::ostringstream strs;
        strs << jsn[item];
        return strs.str();
    }
    return jsn[item].get<std::string>();
}

double optirepair::orutils::convertAccelerationToSpeed(double acceleration, double rotation) {
    if (rotation == 0)
        return 0;
    return acceleration / (2 * CONST_G * rotation) * 1000;
}

long long optirepair::orutils::timeInMilliseconds() {
    struct timeval tv{};

    gettimeofday(&tv,nullptr);
    return (static_cast<long long>(tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

double optirepair::orutils::parseStringWithDouble(const std::string &value) {
    double ret = 0;
    double pos = 1;
    bool beforeSign = true;

    for (auto const &val: value) {
        if (val >= '0' && val <= '9') {
            if (beforeSign) {
                ret *= 10.0;
            } else {
                pos /= 10.0;
            }
            ret += (val - '0') * pos;
            continue;
        }
        if (val == ',' || val == '.') {
            if (beforeSign) {
                beforeSign = false;
            } else {
                return ret;
            }
            continue;
        }
        if (val == ' ') {
            continue;
        }
        return ret;
    }

    return ret;

}

