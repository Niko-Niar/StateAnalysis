#include "ormodulecallback.h"

#include "ordebug.h"

#define UNUSED(x) (void)(x)

using namespace optirepair;

void ORModuleCallback::orGotEvent(const std::string &serviceId, const std::string &message) {
    UNUSED(serviceId);
    UNUSED(message);
    ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Default handler was called. You can override it");
}

void ORModuleCallback::orGotAction(const std::string &doc) {
    UNUSED(doc);
    ordebug::info(__PRETTY_FUNCTION__, __LINE__, "Default handler was called. You can override it");
}

void ORModuleCallback::orGotOtherConfigs(const std::string &dataType, const std::map<std::string, DataBlock *>& vector) {
    std::ignore = vector;
    std::ignore = dataType;
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Default handler was called. You can override it");
}

void ORModuleCallback::orGotStorageData(long long timestamp, DataQuery query, DataBlock *content) {
    std::ignore = query;
    std::ignore = timestamp;
    delete content;
    ordebug::info(__PRETTY_FUNCTION__ , __LINE__, "Default handler was called. You can override it");
}
