//
// Created by kulakov on 19.05.2021.
//

#include "IBrocker.h"

#include "MQTTBrocker.h"

#include <stdexcept>

//TODO: https://tldp.org/HOWTO/html_single/C++-dlopen/
IBrocker * IBrocker::getBrocker(const std::string &brokerName) {
    if (brokerName == "mqtt") {
        return new MQTTBrocker();
    } else {
        throw std::runtime_error("Unknown brocker type " + brokerName);
    }
}
