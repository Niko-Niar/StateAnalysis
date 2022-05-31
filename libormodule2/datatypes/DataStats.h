//
// Created by kulakov on 22.06.2021.
//

#ifndef OR_ADC_SOURCE_MODULE_DATASTATS_H
#define OR_ADC_SOURCE_MODULE_DATASTATS_H

class DataStats : public DataBlock {
public:
    DataStats() = default;

    DataStats(long total, long losses) {
        setValue("total", total);
        setValue("losses", losses); }

protected:
    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"total", OR_TYPE_LONG},
            {"losses", OR_TYPE_LONG}};
};

#endif //OR_ADC_SOURCE_MODULE_DATASTATS_H
