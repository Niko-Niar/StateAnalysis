#ifndef RECORD_H_INCLUDED
#define RECORD_H_INCLUDED
#include <vector>
#include <string>
#include "Field.h"
#include "Subscription.h"
#include <nlohmann/json.hpp>

/// Статус записи и каждого поля записи
enum Status {EMPTY, FULL};
/** \brief "Класс, хранящий коллекцию полей данных"
 *
 * Класс, хранящий коллекцию полей данных, пришедших от некоторого сервиса. Позволяет интерполировать данные полей
 */
class Record {
    Subscription *master_sub = nullptr;//подписка, к которой принадлежит запись
    std::vector<Field> fields{};//вектор полей
    int64_t timestamp = 0;//временная метка записи
    Status cur_status = EMPTY;//текущий статус записи

    //запрещаем 
    Record(const Record &T) = delete;
    Record& operator=(const Record &T) = delete;
public:
    friend class URecord;
    ///конструктор 
    Record() {}
    /** \brief Прикрепляет запись к конкретной подписке.
     *
     * \param sub - указатель на объект - подписку
     * \throw В случае двойного вызова генерирует исключение типа string
     */
    void init(Subscription *sub);
    /** \brief Перезаписывает поля данных записи.
     *
     * Перезаписывает поля данных записи в соответствии с json - объектом data.
     * \param data_object - ссылка на json - объект с полями данных
     * \param timestamp - временная метка сообщения
     * \return В случае успеха true, в противном случае false
     * \throw В случае нераспознанного типа поля элемента коллекции fields генерируется исключение типа string
     */
    bool set_data(nlohmann::json data, int64_t timestamp);
    /** \brief Устанавливает статус записи и каждого поля записи в состояние EMPTY.
     *
     * \param timestamp - временная метка записи
     */
    void set_as_empty(int64_t timestamp);
    /** \brief Заполняет поля записи на основе интерполяции.
     *
     * \param r1 - ссылка на предыдущую по времени заполненную запись
     * \param r2 - ссылка на последующую по времени запоненную запись
     * \throw В случае некоректных: временных меток, принадлежности записей подпискам, типов полей элементов коллекции fields
     * генерируется исключение типа string
     */
    void calc_poly(Record &r1, Record &r2);
    /** \brief Возвращает значение временной метки записи.
     *
     * \return Значение временной метки записи
     */
    inline int64_t get_timestamp() const{return timestamp;}
    /** \brief Возвращает текущий статус записи.
     *
     * \return Текущий статус записи
     */
    inline Status get_cur_status(){return cur_status;}
};
#endif // RECORD_H_INCLUDED
