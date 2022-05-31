//
// Created by demo on 20.10.2020.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_UTILS_H
#define ROOT_OR_SERVER_MONITOR_MODULE_UTILS_H

#include <cstring>
#include <nlohmann/json.hpp>

namespace optirepair {
    class orutils {
    public:
        constexpr static const double CONST_G = 9.81;

        /**
         * String округление значения до указанного знака после запятой
        * @param value значение
        * @param precision кол-во знаков после запятой
        * @return округленная строка
        */
        static std::string str_round(double value, int precision);

        static std::string str_round(float value, int precision);

        /**
         * получение вещественного значения из json документа
         * @param jsn json документ
         * @param item поле со значением
         * @return значение
         */
        static double getDouble(nlohmann::json jsn, const std::string &item, double def = 0);

        /**
         * получение целого значения из json документа
         * @param jsn json документ
         * @param item поле со значением
         * @return значение
         */
        static int getInt(nlohmann::json jsn, const std::string &item, int def = 0);

        /**
         * получение строки из json документа
         * @param jsn json документ
         * @param item поле со значением
         * @return значение
         */
        static std::string getString(nlohmann::json jsn, const std::string &item);

        static double parseStringWithDouble(const std::string& value);

        /**
         * сравнение двух строк с игнором
         * @param a строка 1
         * @param b строка 2
         * @return истина если строки одинаковы
         */
        static bool iequals(const std::string &a, const std::string &b);

        /**
         * перевод значения из виброускорения в виброскорость по формуле Vi = Ai /(2*3.14*fi) *1000
         * @param acceleration значение виброускорения
         * @param rotation частота вращения
         * @return значение виброскорости
         */
        static double convertAccelerationToSpeed(double acceleration, double rotation);

        /**
         * получение текущего времени в миллисекундах
         * @return время в миллисекундах
         */
        static long long int timeInMilliseconds();
    };
}


#endif //ROOT_OR_SERVER_MONITOR_MODULE_UTILS_H
