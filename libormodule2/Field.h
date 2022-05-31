#ifndef FIELD_H_INCLUDED
#define FIELD_H_INCLUDED
#include "ordebug.h"
#include <string>
#include <stdexcept>

#define MIN_INT64_T_POLY_DEG 0
#define MAX_INT64_T_POLY_DEG 1

#define MIN_STRING_POLY_DEG 0
#define MAX_STRING_POLY_DEG 0

#define MIN_DOUBLE_POLY_DEG 0
#define MAX_DOUBLE_POLY_DEG 1

/// Возможные типы полей
enum Field_types {INT64_T, STRING, DOUBLE};

/** \brief Класс, описывающий поле данных.
 *
 * Класс, описывающий поле данных, которое может иметь любой из типов, перечисленых в Field_types.
 */
class Field {
    const std::string name;
    const Field_types type;
    int64_t Ivalue = 0;
    std::string Svalue = "";
    double Dvalue = 0.0;
public:
    /** \brief Конструктор
     *
     * \param name - имя поля
     * \param type - тип поля
     */
    Field(const std::string &name, Field_types type) : name(name), type(type) {}
    /** \brief Возвращает имя поля
     *
     * \return Имя поля
     */
    inline const std::string& get_name(){return name;}
    /** \brief Возвращет тип поля
     *
     * \return Тип поля
     */
    inline Field_types get_type(){return type;}
    /** \brief Возвращает ссылку на значение поля
     *
     * \return Cсылку на значение поля типа int64_t
     * \throw В случае, если поле имеет тип, отличный от int64_t, генерируется исключение типа string
     */
    inline int64_t& get_Ivalue(){
        if(type != INT64_T)
            throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Wrong type!"));
        return Ivalue;
    }
    /** \brief Возвращает ссылку на значение поля
     *
     * \return Cсылку на значение поля типа string
     * \throw В случае, если поле имеет тип, отличный от string, генерируется исключение типа string
     */
    inline std::string& get_Svalue(){
        if(type != STRING)
            throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Wrong type!"));
        return Svalue;
    }
    /** \brief Возвращает ссылку на значение поля
     *
     * \return Cсылку на значение поля типа double
     * \throw В случае, если поле имеет тип, отличный от double, генерируется исключение типа string
     */
    inline double& get_Dvalue(){
        if(type != DOUBLE)
            throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Wrong type!"));
        return Dvalue;
    }
};
#endif // FIELD_H_INCLUDED
