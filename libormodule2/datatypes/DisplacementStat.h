//
// Created by besednyi on 10.09.2021.
//

#ifndef ROOT_OR_SERVER_MONITOR_MODULE_DISPLACEMENTSTAT_H
#define ROOT_OR_SERVER_MONITOR_MODULE_DISPLACEMENTSTAT_H

#include "../DataBlock.h"

class DisplacementStat : public DataBlock {
protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"value_x", OR_TYPE_DOUBLE},
            {"value_y", OR_TYPE_DOUBLE},
            {"value_z", OR_TYPE_DOUBLE}
    };
};

#endif //ROOT_OR_SERVER_MONITOR_MODULE_DISPLACEMENTSTAT_H
