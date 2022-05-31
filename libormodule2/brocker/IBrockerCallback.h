//
// Created by kulakov on 25.03.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_IBROCKERCALLBACK_H
#define ROOT_OR_SERVER_MONITOR_MODULE_IBROCKERCALLBACK_H

#include <string>

class IBrockerCallback {
public:
    virtual ~IBrockerCallback() = default;

    /**
     * Обработка уведомлений об изменении конфига
     */
    virtual void brockerGotConfigMessage() = 0;

    /**
     * обработка уведомлений о действиях
     * @param action действие
     * @param content параметры действия
     */
    virtual void brockerGotAction(const std::string &action, const std::string &content) = 0;

    /**
     * обработка уведомлений о событиях
     * @param serviceId ID модуля отправителя
     * @param content параметры события в формате json
     */
    virtual void brockerGotEvent(const std::string &serviceId, const std::string &content) = 0;
};

#endif //ROOT_OR_SERVER_MONITOR_MODULE_IBROCKERCALLBACK_H
