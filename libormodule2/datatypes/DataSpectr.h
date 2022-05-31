//
// Created by kulakov on 01.07.2021.
//

#ifndef OR_ADC_SOURCE_MODULE_DATASPECTR_H
#define OR_ADC_SOURCE_MODULE_DATASPECTR_H

#include <DataBlock.h>

#include <utility>

class DataSpectr : public DataBlock {
public:
    DataSpectr() = default;
    DataSpectr(double leftX, double rightX, int size, const std::vector<double>& values, int type) {
        setValue("left_x", leftX);
        setValue("right_x", rightX);
        setValue("size", size);
        setValue("values", values);
        setValue("type", type);
    }

protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"left_x", OR_TYPE_DOUBLE},
            {"right_x", OR_TYPE_DOUBLE},
            {"size", OR_TYPE_INT},
            {"values", OR_TYPE_ARRAY},
            {"type", OR_TYPE_INT}};
};

#endif //OR_ADC_SOURCE_MODULE_DATASPECTR_H
