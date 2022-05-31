//
// Created by kulakov on 23.07.2021.
//

#ifndef OR_TAHO_MODULE_DATATAHO_H
#define OR_TAHO_MODULE_DATATAHO_H

#include <DataBlock.h>

class DataTaho : public DataBlock {
public:
    DataTaho(double min, double mean, double max, int direction) {
        setValue("min", min);
        setValue("value", mean);
        setValue("max", max);
        setValue("direction", direction);
    }

protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"min", OR_TYPE_DOUBLE},
            {"value", OR_TYPE_DOUBLE},
            {"max", OR_TYPE_DOUBLE},
            {"direction", OR_TYPE_INT}
    };
};

#endif //OR_TAHO_MODULE_DATATAHO_H
