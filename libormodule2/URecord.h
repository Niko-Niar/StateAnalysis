#ifndef URECORD_H_INCLUDED
#define URECORD_H_INCLUDED
#include "Record.h"
#include "ordebug.h"
#include <string>
#include <stdexcept>

/** Класс, предназначенный для выдачи данных пользователю.
 *
 * Класс содержит указатель на вектор полей, хранящийся в каком - либо
 * объекте класса Record. Является дружественным к классу Record.
 */
class URecord{
    std::vector<Field> *fields = nullptr;//указатель на вектор полей
public:
    ///Конструктор
    URecord(Record &record){
        fields = &record.fields;
    }
    ///Конструктор копирования
    URecord(const URecord &T){
        fields = T.fields;
    }
    /** \brief Возвращает значение поля целого типа с именем field_name.
     *
     * \param field_name - имя поля данных.
     * \throw В случае отсутствия поля с именем field_name, генерируется исключение типа
     * std::string. Если реальный тип поля не является int64_t, то будет получено исключение из функции
     * класса Field.
     * \return Целое число.
     */
    int64_t get_int(const std::string &field_name){
        for(std::vector<Field>::iterator it = fields->begin(); it != fields->end(); it++){
            if(it->get_name() == field_name){
                return it->get_Ivalue();
            }
        }
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "No such field exists!"));
    }
    /** \brief Возвращает константную ссылку на строку, содержащуюся в поле с именем field_name.
     *
     * \param field_name - имя поля данных.
     * \throw В случае отсутствия поля с именем field_name, генерируется исключение типа
     * std::string. Если реальный тип поля не является std::string, то будет получено исключение из функции
     * класса Field.
     * \return константную ссылку на строку типа std::string.
     */
    const std::string& get_string(const std::string &field_name){
        for(std::vector<Field>::iterator it = fields->begin(); it != fields->end(); it++){
            if(it->get_name() == field_name){
                return it->get_Svalue();
            }
        }
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "No such field exists!"));
    }
    /** \brief Возвращает значение поля вещественного (double) типа с именем field_name.
     *
     * \param field_name - имя поля данных.
     * \throw В случае отсутствия поля с именем field_name, генерируется исключение типа
     * std::string. Если реальный тип поля не является double, то будет получено исключение из функции
     * класса Field.
     * \return Вещественное число (тип double).
     */
    double get_double(const std::string &field_name){
        for(std::vector<Field>::iterator it = fields->begin(); it != fields->end(); it++){
            if(it->get_name() == field_name){
                return it->get_Dvalue();
            }
        }
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "No such field exists!"));
    }
};
#endif // URECORD_H_INCLUDED
