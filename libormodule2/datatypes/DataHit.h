//
// Created by kulakov on 01.07.2021.
//

#ifndef OR_ADC_SOURCE_MODULE_DATAHIT_H
#define OR_ADC_SOURCE_MODULE_DATAHIT_H

#include <DataBlock.h>

#define DATAHIT_NAME "name"

class DataHit : public DataBlock {
public:
    DataHit() = default;
    DataHit(const std::string& name, const std::string& description, double maxDev, double rms) {
        setValue(DATAHIT_NAME, name);
        setValue("description", description);
        setValue("max_dev", maxDev);
        setValue("rms", rms);
    }

protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {DATAHIT_NAME, OR_TYPE_STRING},
            {"description", OR_TYPE_STRING},
            {"max_dev", OR_TYPE_DOUBLE},
            {"rms", OR_TYPE_DOUBLE},
            {"sensor_id", OR_TYPE_STRING}
    };
};

#endif //OR_ADC_SOURCE_MODULE_DATAHIT_H
