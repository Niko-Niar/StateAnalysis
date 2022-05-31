#include "Subscription.h"
#include "ordebug.h"    
#include <stdexcept>

Subscription::Subscription(const std::string &serviceId, const std::string &message_type) : serviceId(serviceId), message_type(message_type) {}
void Subscription::add_field(const std::string &field_name, Field_types field_type, int field_poly_deg){
    if(is_stop_add_fields){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Adding new fields is no longer possible!"));
    }
    for(std::vector<std::string>::iterator it = fields_name.begin(); it != fields_name.end(); it++){
        if(*it == field_name)
            throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "This field already exists!"));
    }
    switch(field_type){
        case INT64_T:
            if((field_poly_deg < MIN_INT64_T_POLY_DEG) || (field_poly_deg > MAX_INT64_T_POLY_DEG)){
                throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Polynomial degree is incorrect!"));
            }
            fields_name.push_back(field_name);
            fields_type.push_back(field_type);
            fields_poly_deg.push_back(field_poly_deg);
        break;

        case STRING:
            if((field_poly_deg < MIN_STRING_POLY_DEG) || (field_poly_deg > MAX_STRING_POLY_DEG)){
                throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Polynomial degree is incorrect!"));
            }
            fields_name.push_back(field_name);
            fields_type.push_back(field_type);
            fields_poly_deg.push_back(field_poly_deg);
        break;

        case DOUBLE:
            if((field_poly_deg < MIN_DOUBLE_POLY_DEG) || (field_poly_deg > MAX_DOUBLE_POLY_DEG)){
                throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Polynomial degree is incorrect!"));
            }
            fields_name.push_back(field_name);
            fields_type.push_back(field_type);
            fields_poly_deg.push_back(field_poly_deg);
        break;

        default:
            throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Wrong type!"));
        break;
    }
}
void Subscription::end_add_fields(){
    if(is_stop_add_fields)
        return;
    if(fields_name.size() == 0)
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "The number of fields is zero!"));
    is_stop_add_fields = true;
}
