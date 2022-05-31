//
// Created by kulakov on 30.06.2021.
//

#ifndef OR_ADC_SOURCE_MODULE_DATABOUNDARY_H
#define OR_ADC_SOURCE_MODULE_DATABOUNDARY_H

#include "../DataBlock.h"

enum BoundaryZones {
    NORMAL_ZONE = 0,
    WARNING_ZONE = 1,
    DANGER_ZONE = 2,
    ERROR_ZONE = 3
};

class DataBoundary : public DataBlock {
public:
    DataBoundary(double be, double bd, double bw,
                 double tw, double td, double te,
                 double value, int type, BoundaryZones zone) {

            setValue("value", value);
            setValue("bottom_error_boundary", be);
            setValue("bottom_danger_boundary", bd);
            setValue("bottom_warning_boundary", bw);
            setValue("top_warning_boundary", tw);
            setValue("top_danger_boundary", td);
            setValue("top_error_boundary", te);
            setValue("type", type);
            setValue("action", zone);

    };

    /**
     * Возвращает зону, в которой находится значение value
     * @param be
     * @param bd
     * @param bw
     * @param tw
     * @param td
     * @param te
     * @param value
     * @return
     */
    static BoundaryZones check(double be, double bd, double bw,
                               double tw, double td, double te,
                               double value) {
        BoundaryZones zone = BoundaryZones::ERROR_ZONE;
        if (value < tw && value > bw) {
            zone = BoundaryZones::NORMAL_ZONE;
        } else if (value < td && value > bd) {
            zone = BoundaryZones::WARNING_ZONE;
        } else if (value < te && value > be) {
            zone = BoundaryZones::DANGER_ZONE;
        }
        return zone;
    }

protected:

    std::map<std::string, OR_DATA_TYPES> getBlockDescription() const override {
        return m_items;
    }

    std::map<std::string, OR_DATA_TYPES> m_items = {
            {"value", OR_TYPE_DOUBLE},
            {"top_error_boundary", OR_TYPE_DOUBLE},
            {"top_danger_boundary", OR_TYPE_DOUBLE},
            {"top_warning_boundary", OR_TYPE_DOUBLE},
            {"bottom_error_boundary", OR_TYPE_DOUBLE},
            {"bottom_danger_boundary", OR_TYPE_DOUBLE},
            {"bottom_warning_boundary", OR_TYPE_DOUBLE},
            {"type", OR_TYPE_INT},
            {"action", OR_TYPE_INT}
    };
};

#endif //OR_ADC_SOURCE_MODULE_DATABOUNDARY_H
