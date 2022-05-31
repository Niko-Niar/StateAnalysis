#include "Record.h"
#include "ordebug.h"
#include <stdexcept>

void Record::init(Subscription *sub){
    if(master_sub != nullptr){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Double call!"));
    }
    master_sub = sub;
    for(int64_t i = 0; i < sub->get_num_fields(); i++){
        fields.push_back(Field(sub->get_field_name(i), sub->get_field_type(i)));
    }
}
bool Record::set_data(nlohmann::json data, int64_t timestamp){
    int64_t i, n_fields = static_cast<int64_t>(fields.size());
    this->timestamp = timestamp;//временная метка должна быть новой вне зависимости от результата выполнения
    cur_status = EMPTY;
    try{
        for(i = 0; i < n_fields; i++){
            switch(fields[i].get_type()){
                case INT64_T:
                    fields[i].get_Ivalue() = data[fields[i].get_name()].get<int64_t>();
                break;

                case STRING:
                    fields[i].get_Svalue() = data[fields[i].get_name()].get<std::string>();
                break;

                case DOUBLE:
                    fields[i].get_Dvalue() = data[fields[i].get_name()].get<double>();
                break;

                default:
                    optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Invalid field type!");
                    return false;
            }
        }
    }catch(...){
        optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, std::string("Error with field: ") + fields[i].get_name() + 
            std::string(". Subscription: ") + master_sub->get_serviceId() + std::string(". Missing or something other."));
        return false;
    }
    cur_status = FULL;
    return true;
}
void Record::set_as_empty(int64_t timestamp){
    cur_status = EMPTY;
    this->timestamp = timestamp;
}
void Record::calc_poly(Record &r1, Record &r2){
    if((master_sub != r1.master_sub) || (master_sub != r2.master_sub)){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Record ownership inconsistency!"));
    }
    if(r1.cur_status == EMPTY){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "The current status of record r1 is empty!"));
    }
    if(r2.cur_status == EMPTY){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "The current status of record r2 is empty!"));
    }
    if(r1.timestamp >= r2.timestamp){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Record r1.timestamp >= Record r2.timestamp!"));
    }
    if(timestamp < r1.timestamp){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, 
            "The timestamp of the current record is less than the timestamp of the record r1!"));
    }
    if(timestamp > r2.timestamp){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, 
            "The timestamp of the current record is greater than the timestamp of the record r2!"));
    }

    int64_t i, n_fields = static_cast<int64_t>(fields.size());
    for(i = 0; i < n_fields; i++){
        if(master_sub->get_field_poly_deg(i) == 0){
             switch(fields[i].get_type()){
                case INT64_T:
                    fields[i].get_Ivalue() = r1.fields[i].get_Ivalue();
                break;

                case DOUBLE:
                    fields[i].get_Dvalue() = r1.fields[i].get_Dvalue();
                break;

                case STRING:
                     fields[i].get_Svalue() = r1.fields[i].get_Svalue();
                break;

                default:
                    throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Invalid field type!"));
            }
        }else if(master_sub->get_field_poly_deg(i) == 1){
            switch(fields[i].get_type()){
                case INT64_T:
                    fields[i].get_Ivalue() = (timestamp - r1.timestamp)*(r2.fields[i].get_Ivalue() - r1.fields[i].get_Ivalue())/
                                             (r2.timestamp - r1.timestamp) + r1.fields[i].get_Ivalue();
                break;

                case DOUBLE:
                    fields[i].get_Dvalue() = static_cast<double>(timestamp - r1.timestamp) *  (r2.fields[i].get_Dvalue() - r1.fields[i].get_Dvalue())/
                                             static_cast<double>(r2.timestamp - r1.timestamp) + r1.fields[i].get_Dvalue();
                break;

                default:
                    throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Invalid field type!"));
            }
        }else{
            throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Incorrect interpolation degree!"));
        }
    }
    cur_status = FULL;
}
