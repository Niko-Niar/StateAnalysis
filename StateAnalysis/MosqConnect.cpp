#pragma comment(lib, "mosquitto.lib")

#include <stdio.h>
#include <mosquitto.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "BSAnalysis.h"

using namespace std;

int k = 0;
vector<float> v_1;
vector<float> v_2;

void my_message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
	string str;
	string path;
	int n;
	if(message->payloadlen && k == 0) {
		str.append((char*)message->payload);
		n = str.find("store_next_record");
		if (n > 0)
		{
			k = 1;
			printf("Начало записи\n");
		}
	}
	else if (message->payloadlen && k == 1) {
		str.append((char*)message->payload);
		n = str.find("taho_record_ended");
		if (n > 0)
		{
			k = 0;
			path.append(str.erase(0, str.find("file") + 5));
			path.erase(path.find("}"));
			printf("Конец записи и путь: %s\n", path.c_str());
			ifstream in(path);
			string number;
			if (in.is_open())
			{
				while (getline(in, number, ' '))
				{
					v_2.push_back(atoi(number.c_str()));
					/*printf("%d\n", atoi(number.c_str()));*/
				}
			}
			in.close();

			Interpolation(v_1, v_2);
			v_1.clear();
			v_2.clear();
		}
		else
		{
			str.erase(0, str.find("value") + 6);
			str.erase(str.find("}"));
			printf("Записано значение: %s\n", str.c_str());
			v_1.push_back(atoi(str.c_str()));
		}
	}
	else {
		printf("%s (null)\n", message->topic);
	}
	fflush(stdout);
}

void my_connect_callback(struct mosquitto* mosq, void* userdata, int result)
{
	if (!result) {
		/* Подписка на тему */
		mosquitto_subscribe(mosq, NULL, "test", 2);
	}
	else {
		fprintf(stderr, "Сбой подключения\n");
	}
}

void my_subscribe_callback(struct mosquitto* mosq, void* userdata, int mid, int qos_count, const int* granted_qos)
{
	printf("Подключение прошло успешно\n");// Вывод сообщения об успешном подключении
}

int mosq_connect()
{
	setlocale(LC_ALL, "Russian");
	const char* host = "localhost";
	int port = 1883;
	int keepalive = 60;
	bool clean_session = false;
	struct mosquitto* mosq = NULL;

	mosquitto_lib_init();

	mosq = mosquitto_new("1", clean_session, NULL);

	if (!mosq) {
		fprintf(stderr, "Ошибка: Недостаточно памяти.\n");
		return 1;
	}
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_message_callback_set(mosq, my_message_callback);
	mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

	if (mosquitto_connect(mosq, host, port, keepalive)) {
		fprintf(stderr, "Не удается подключиться.\n"); // Вывод ошибки о неудачной попытке подключения
		return 1;
	}

	while (!mosquitto_loop(mosq, -1, 1)) {
	}
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}