//
// Created by kulakov on 14.04.2021.
//

#ifndef ORMODULE2_TESTS_DEFAULTBROCKER_H
#define ORMODULE2_TESTS_DEFAULTBROCKER_H

#include "IBrocker.h"

class DefaultBrocker : public IBrocker {
public:
    ~DefaultBrocker() override;

    void init(const std::string& clusterName, const std::string& serviceId, const std::string& config, IBrockerCallback *callback) override;

    void sendNotification(long long int timestamp, int level, const std::string &eventId, const std::string &sender,
                          const std::string &type, DataBlock *data) override;

    void sendErrorEvent(long long int timestamp, int errorCode, const std::string& message, bool stopPropagation) override;

    void sendActionResponse(long long int timestamp, const std::string &type, const std::string &sender, DataBlock *data) override;

    void sendActionRequest(long long int timestamp, const std::string& serviceId, const std::string &type, const std::string &sender, DataBlock *data) override;

    void subscribe(const std::string& serviceId) override;

    void unsubscribe(const std::string& serviceId) override;

    void sendConfigNotification(const std::string &sender, const std::string &data, bool stopPropagation) override;
    
    void sub_controller() override; 
};


#endif //ORMODULE2_TESTS_DEFAULTBROCKER_H
