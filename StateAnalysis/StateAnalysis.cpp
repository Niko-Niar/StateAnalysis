#include "ORModule.h"
#include <iostream>
#include <uv.h>

int MosqConnect();

using namespace std;

int main(int argc, char** argv) {
    /* Проверка на получение аргументов (id и config.json) */
    /*if (argc < 2) {
        cerr << "usage: " << argv[0] << " " << "service_id" << endl;
        exit(1);
    }*/

    /*for (int i = 1; i < argc; i++) {
        cout << "argv[" << i << "] - " << argv[i] << endl;
    }*/
    
    MosqConnect();

    return 0;
}