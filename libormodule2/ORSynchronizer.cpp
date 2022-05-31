#include "ORSynchronizer.h"
#include "ORModule.h"      
#include "ordebug.h"
#include <nlohmann/json.hpp>
#include <stdexcept>

Subscription& ORSynchronizer::add_sub_to_synch(const std::string &serviceId, const std::string &type){
    if(is_end_add_sub){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__,  __LINE__, "Adding new subscriptions is no longer possible!"));
    }

    int64_t i, n_sub = static_cast<int64_t>(subscriptions.size());
    if(n_sub > 0){
        subscriptions[n_sub - 1].end_add_fields();
        for(i = 0; i < n_sub; i++){
            if((subscriptions[i].get_serviceId() == serviceId) && (subscriptions[i].get_message_type() == type))
                throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "This subscription already exists!"));
        }
    }
   
    for(i = 0; i < n_sub; i++){
        if(subscriptions[i].get_serviceId() == serviceId){
            break;
        }
    }
    if(i == n_sub){
        service_ids.push_back(serviceId);
    }
    
    subscriptions.push_back(Subscription(serviceId, type));
    return subscriptions[subscriptions.size() - 1];
}
void ORSynchronizer::end_add_sub_to_synch(){
    if(is_end_add_sub)
        return;
    int64_t i, j, n_sub = static_cast<int64_t>(subscriptions.size());
    if(n_sub == 0){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "The number of subscriptions is zero!"));
    }
    is_end_add_sub = true;

    subscriptions[n_sub - 1].end_add_fields();

    //создаём матрицу записей
    records = new Record* [DEPTH_HISTORY];
    for(i = 0; i < DEPTH_HISTORY; i++){
        records[i] = new Record[n_sub];
        for(j = 0; j < n_sub; j++){
            records[i][j].init(&(subscriptions[j]));
        }
    }
    //подписываемся
    for(i = 0; i < static_cast<int64_t>(service_ids.size()); i++){
        ORModule::subscribeToService(service_ids[i]);
    }
}
void ORSynchronizer::event_notification_to_synch(const std::string &serviceId, const std::string &message){
    if(records == nullptr){
        throw std::logic_error(optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Synchronizer::end_add_sub_to_synch() was not called!"));
    }
    int64_t i, z, j, k, n_sub = static_cast<int64_t>(subscriptions.size());
    nlohmann::json json_parse;
    int64_t timestamp;
    std::string type;
    nlohmann::json data;
    try{
        json_parse = nlohmann::json::parse(message);
        timestamp = json_parse["timestamp"].get<int64_t>();
        type = json_parse["type"].get<std::string>();
        data = json_parse["data"].get<nlohmann::json>();
    }catch(...){
        optirepair::ordebug::error(__PRETTY_FUNCTION__, __LINE__, "Incorrect json message!");
        return;
    }
    
    if(timestamp > records[0][0].get_timestamp()){
        //сдвигаем строки матрицы записей
        Record *ptr_mem = records[DEPTH_HISTORY - 1];
        for(i = DEPTH_HISTORY - 1; i > 0; i--){
            records[i] = records[i - 1];
        }
        records[0] = ptr_mem;

        //сдвигаем номер записи для предполагаемой посылки
        if(id_next_send < DEPTH_HISTORY - 1){
            id_next_send++;
        }

        //обработка
        for(i = 0; i < n_sub; i++){
            if((subscriptions[i].get_serviceId() == serviceId) && (subscriptions[i].get_message_type() == type)){
                if(!records[0][i].set_data(data, timestamp))
                    continue;//будет пустая запись с новой временной меткой
                //интерполяция полей
                for(j = 1; j < DEPTH_HISTORY; j++){
                    if(records[j][i].get_cur_status() == FULL){
                        for(k = j - 1; k > 0; k--){
                            records[k][i].calc_poly(records[j][i], records[0][i]);
                        }
                        break;
                    }
                }
            }else {
                records[0][i].set_as_empty(timestamp);
            }
        }
    }else if(timestamp == records[0][0].get_timestamp()){
        if(id_next_send == -1)//-1 означает, что самую верхнюю строку матрицы мы уже выдали
            return;
        for(i = 0; i < n_sub; i++){
            if((subscriptions[i].get_serviceId() == serviceId) && (subscriptions[i].get_message_type() == type)){
                if(records[0][i].get_cur_status() == FULL)
                    return;
                if(!records[0][i].set_data(data, timestamp))
                    return;
                //интерполяция полей
                for(j = 1; j < DEPTH_HISTORY; j++){
                    if(records[j][i].get_cur_status() == FULL){
                        for(k = j - 1; k > 0; k--){
                            records[k][i].calc_poly(records[j][i], records[0][i]);
                        }
                        break;
                    }
                }
                break;
            }
        }
    }else{
        for(z = 1; z < DEPTH_HISTORY; z++){
            if(records[z][0].get_timestamp() == timestamp){
                for(i = 0; i < n_sub; i++){
                    if((subscriptions[i].get_serviceId() == serviceId) && (subscriptions[i].get_message_type() == type)){
                        if((records[z][i].get_cur_status() == FULL) || (z > id_next_send))
                            return;
                        if(!records[z][i].set_data(data, timestamp))
                            return;
                        //интерполяция полей
                        //снизу
                        for(j = z + 1; j < DEPTH_HISTORY; j++){
                            if(records[j][i].get_cur_status() == FULL){
                                for(k = j - 1; k > z; k--){
                                    records[k][i].calc_poly(records[j][i], records[z][i]);
                                }
                                break;
                            }
                        }
                        //сверху
                        for(j = z - 1; j >= 0; j--){
                            if(records[j][i].get_cur_status() == FULL){
                                for(k = j + 1; k < z; k++){
                                    records[k][i].calc_poly(records[z][i], records[j][i]);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }else if(records[z][0].get_timestamp() < timestamp){//между z и z - 1
                if(id_next_send < (z - 1))
                    return;
                //сдвигаем строки матрицы записей
                Record *ptr_mem = records[DEPTH_HISTORY - 1];
                for(i = DEPTH_HISTORY - 1; i > z; i--){
                    records[i] = records[i - 1];
                }
                records[z] = ptr_mem;

                //сдвигаем номер записи для предполагаемой посылки
                if(id_next_send < DEPTH_HISTORY - 1){
                    id_next_send++;
                }
                //теперь z - я строка, это вставленная строка
                for(i = 0; i < n_sub; i++){
                    if((subscriptions[i].get_serviceId() == serviceId) && (subscriptions[i].get_message_type() == type)){
                        if(!records[z][i].set_data(data, timestamp))
                            continue;//будет пустая запись с новой временной меткой
                        //интерполяция полей
                        //снизу
                        for(j = z + 1; j < DEPTH_HISTORY; j++){
                            if(records[j][i].get_cur_status() == FULL){
                                for(k = j - 1; k > z; k--){
                                    records[k][i].calc_poly(records[j][i], records[z][i]);
                                }
                                break;
                            }
                        }
                        //сверху
                        for(j = z - 1; j >= 0; j--){
                            if(records[j][i].get_cur_status() == FULL){
                                for(k = j + 1; k < z; k++){
                                    records[k][i].calc_poly(records[z][i], records[j][i]);
                                }
                                break;
                            }
                        }
                    }else{
                        records[z][i].set_as_empty(timestamp);
                        if((z + 1 < DEPTH_HISTORY) && (z - 1 >= 0)){
                            if((records[z + 1][i].get_cur_status() == FULL) && (records[z - 1][i].get_cur_status() == FULL)){
                                records[z][i].calc_poly(records[z + 1][i], records[z - 1][i]);
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    //попытка выдачи
    for(i = id_next_send; i >= 0; i--){
        for(j = 0; j < n_sub; j++){
            if(records[i][j].get_cur_status() == EMPTY){
                break;
            }
        }
        if(j == n_sub){
            urecords.clear();
            for(k = 0; k < n_sub; k++){
                urecords.push_back(URecord(records[i][k]));
            }
            got_synchronized_message(urecords, records[i][0].get_timestamp());
            id_next_send = i - 1;
        }
    }
}
void ORSynchronizer::reset_synch(){
    subscriptions.clear();
    for(int64_t i = 0; i < static_cast<int64_t>(service_ids.size()); i++){
        ORModule::unsubscribeToService(service_ids[i]);
    }
    service_ids.clear();
    is_end_add_sub = false;
    if(records != nullptr){
        for(size_t i = 0; i < DEPTH_HISTORY; i++)
            delete[] records[i];
        delete[] records;
        records = nullptr;
    }
    id_next_send = DEPTH_HISTORY - 1;
    urecords.clear();
}
ORSynchronizer::~ORSynchronizer(){
    int64_t i;
    for(i = 0; i < static_cast<int64_t>(service_ids.size()); i++){
        ORModule::unsubscribeToService(service_ids[i]);
    }
    if(records != nullptr){
        for(i = 0; i < DEPTH_HISTORY; i++)
            delete[] records[i];
        delete[] records;
    }
}
