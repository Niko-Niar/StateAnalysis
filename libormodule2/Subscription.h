#ifndef SUBSCRIPTION_H_INCLUDED
#define SUBSCRIPTION_H_INCLUDED
#include <string>
#include <vector>
#include <map>
#include "Field.h"

///Класс, описывающий подписку

class Subscription {
    const std::string serviceId{};
    const std::string message_type{};
    bool is_stop_add_fields = false;//флаг окончания добавления полей
    std::vector<std::string> fields_name{};//имена полей
    std::vector<Field_types> fields_type{};//типы полей
    std::vector<int> fields_poly_deg{};//степени интерполирующих полиномов полей
public:
    /** \brief Конструктор.
     *
     * \param serviceId - id сервиса, на который мы подписаны
     * \param message_type - тип сообщения (значение поля type в json сообщении)
     */
    Subscription(const std::string &serviceId, const std::string &message_type);
    /** \brief Добавляет новое поле к подписке.
     *
     * \param field_name - имя добавляемого поля
     * \param field_type - тип добавляемого поля
     * \param field_poly_deg - степень интерполирующего полинома
     * \throw В случаях повторного добавления подписки, указания неверного типа подписки, некорректного
     * значения степени полинома, вызова метода после вызова метода add_field генерируется исключение типа string.
     */
    void add_field(const std::string &field_name, Field_types field_type, int field_poly_deg);
    /** \brief Вызов этого метода запрещает добавление новых полей.
     *
     * Вызов этого метода запрещает добавление новых полей. Вызывать этот метод явно не нужно. Он будет
     * вызван автоматически при добавлении следующей подписки.
     * \throw В случае, если на момент вызова этого метода не было добавлено ни одного поля, будет
     * сгенерировано исключение типа string. Возможность добавления новых полей остаётся.
     */
    void end_add_fields();
    /** \brief Возвращает Id сервиса, на который мы подписаны.
     *
     * \return Id сервиса, на который мы подписаны.
     */
    inline const std::string& get_serviceId() {return serviceId;}
    /** \brief Возвращает тип сообщения, которое отслеживается.
     *
     * \return Тип сообщения, которое отслеживается.
     */
    inline const std::string& get_message_type() {return message_type;}
    /** \brief Возвращает количество отслеживаемых полей.
     *
     * \return Количество отслеживаемых полей.
     */
    inline int64_t get_num_fields(){return fields_name.size();}
    /** \brief Возвращает имя поля с индексом index.
     *
     * \param index - индекс поля
     * \return Имя поля
     */
    inline const std::string& get_field_name(size_t index){return fields_name[index];}
    /** \brief Возвращает тип поля с индексом index.
     *
     * \param index - индекс поля
     * \return Тип поля
     */
    inline Field_types get_field_type(size_t index){return fields_type[index];}
    /** \brief Возвращает степень интерполирующего полинома для поля с индексом index.
     *
     * \param index - индекс поля
     * \return Cтепень интерполирующего полинома
     */
    inline int get_field_poly_deg(int64_t index){return fields_poly_deg[index];}
};
#endif // SUBSCRIPTION_H_INCLUDED
