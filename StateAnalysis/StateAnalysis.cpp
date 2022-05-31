#include "ORModule.h"
#include <iostream>
#include <uv.h>

int mosq_connect();

using namespace std;

int main(int argc, char** argv) {
    //if (argc < 2) {
    //    cerr << "usage: " << argv[0] << " " << "service_id" << endl;
    //    exit(1);
    //}

    ///*создаем сервис и подключаем его к БД*/
    //if (argc == 2) {
    //    ORModule::init("state-analysis-module", argv[1], ProcessService::getInstance());
    //}
    //else {
    //    ORModule::init("state-analysis-module", argv[1], ProcessService::getInstance(), argv[2]);
    //}
    //uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    mosq_connect();

    return 0;
}