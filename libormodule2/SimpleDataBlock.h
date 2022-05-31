//
// Created by kulakov on 16.04.2021.
//

#ifndef OR_ADC_SOURCE_MODULE_SIMPLEDATABLOCK_H
#define OR_ADC_SOURCE_MODULE_SIMPLEDATABLOCK_H

#include "DataBlock.h"

class SimpleDataBlock : public DataBlock {
public:
    SimpleDataBlock() = default;

    explicit SimpleDataBlock(const std::string &key, OR_DATA_TYPES type = OR_TYPE_STRING) {
        m_name = key;
        m_type = type;
    }

protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        std::map<std::string, OR_DATA_TYPES> m_items;
        m_items[m_name] = m_type;
        return m_items;
    }

    std::string m_name = "value";

    OR_DATA_TYPES m_type = OR_TYPE_STRING;
};

#endif //OR_ADC_SOURCE_MODULE_SIMPLEDATABLOCK_H
