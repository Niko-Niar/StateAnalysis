#ifndef SYNCHRONIZER_H_INCLUDED
#define SYNCHRONIZER_H_INCLUDED
#include <string>
#include <vector>
#include "Subscription.h"
#include "Record.h"
#include "URecord.h"

#define DEPTH_HISTORY 1200

///Класс - синхронизатор сообщений
class ORSynchronizer {
    std::vector<Subscription> subscriptions{};//коллекция подписок (подписка идентифицируется Id сервиса и полем type сообщения)
    std::vector<std::string> service_ids{};//идентификаторы сервисов, на которые нужно подписаться
    bool is_end_add_sub = false;//флаг окончания добавления подписок
    Record **records = nullptr;//матрица записей
    int64_t id_next_send = DEPTH_HISTORY - 1;//номер строки записей, которая ожидается для выдачи
    std::vector<URecord> urecords{};

    ORSynchronizer(const ORSynchronizer &T) = delete;
    ORSynchronizer& operator=(const ORSynchronizer &T) = delete;
public:
    ///Конструктор
    ORSynchronizer() {}

    /** \brief Метод добавления подписки.
     *
     * \param serviceId - Id сервиса, на который мы подписаны.
     * \param type - тип отслеживаемого сообщения (поле type в json сообщении)
     * \return Ссылку на созданную подписку. С её помощью можно добавлять поля.
     * \throw В случаях повторного добавления подписки, вызова после вызова метода end_add()
     * генерируется исключение типа string
     */
    Subscription& add_sub_to_synch(const std::string &serviceId, const std::string &type);
    /** \brief Вызовом этого метода должно завершаться добавление подписок.
     *
     * \throw В случае, если на момент вызова этого метода не добавлено ни одной подписки,
     * будет сгенерировано исключение типа string. Возможность добавления новых подписок остаётся.
     */
    void end_add_sub_to_synch();
    /** \brief Передаёт объекту синхронизатора поступившее событие
     *
     * \param serviceId Ссылка на строку, содержащую Id сервиса, от которого пришло сообщение
     * \param message Ссылка на строку, содержащую сообщение, поступившее от сервиса с Id serviceId
     * \throw В случае, если ранее не был вызван метод end_add, генерируется исключение типа string
     */
    void event_notification_to_synch(const std::string &serviceId, const std::string &message);
    /** \brief Чисто - виртуальный метод, вызываемый синхронизатором для передачи готового сообщения
     *
     * \param records - ссылка на вектор с записями, содержащими значения заказанных полей.
     * \param timestamp - временная метка сообщения.
     */
    virtual void got_synchronized_message(std::vector<URecord> &records, int64_t timestamp) = 0;
    /** \brief Сбрасывает все настройки синхронизатора.
     *
     * Сбрасывает все настройки синхронизатора. Снова можно добавлять подписки и их поля.
     */
    void reset_synch();
    /// Деструктор
    virtual ~ORSynchronizer();
};
#endif // SYNCHRONIZER_H_INCLUDED
