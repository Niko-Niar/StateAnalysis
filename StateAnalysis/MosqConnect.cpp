#pragma comment(lib, "mosquitto.lib")

#include <stdio.h>
#include <mosquitto.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "json.hpp"
#include "Conversion.h"
#include "BSAnalysis.h"

using namespace std;
using json = nlohmann::json;

/* Номер прогона */
int iteration = 0;

/* Флаг сообщения о начале\конце записи */
bool flag = false;

/* Вектора с данными тахометра и датчика вибраций */
vector<float> v_1, v_2;

void DataReading(char* message)
{
	string str, path;
	json object;
	/* Читаем json и парсим */
	str.append(message);
	object = json::parse(str);

	/* Если еще не было сообщения о начале заходим сюда */
	if (flag == false) {
		/*printf("%s\n", str.c_str());*/

		if (object["action"] != nullptr)
		{
			string mes = object["action"];
			if (strcmp(mes.c_str(), "store_next_record") == 0)
			{
				flag = true;
				printf("Начало записи\n");
			}
		}
	}
	/* Если уже получили сообщение о начале */
	else if (flag == true) {
		/*printf("%s\n", str.c_str());*/

		/* Проверяем на сообщение о конце записи */
		if (object["action"] != nullptr)
		{
			string mes = object["action"];
			if (strcmp(mes.c_str(), "taho_record_ended") == 0)
			{
				flag = false;
				path = object["data"]["file"];
				cout << "Конец записи и путь: " << path << endl;

				/* Читаем файлик с данными */
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
				iteration++;

				/* Отправляем вектора на обрезку начала\конца и интерполяцию */
				vector<float> res = Conversion(v_1, v_2);
				v_1.clear();
				v_2.clear();

				/* Отправляем полученный результат на анализ */
				Analysis(res, iteration);
			}
		}
		else
		{
			cout << "Записано значение: " << object["data"]["value"] << endl;
			v_1.push_back(object["data"]["value"]);
		}
	}
}

/* Получение сообщения */
void MessageCallback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
	if(message->payloadlen)
	{
		/* Отправляем сообщение на разбор */
		DataReading((char*)message->payload);
	}
	else
	{
		printf("%s (null)\n", message->topic);
	}
	fflush(stdout);
}

/* Подписка */
void ConnectCallback(struct mosquitto* mosq, void* userdata, int result)
{
	if (!result)
		mosquitto_subscribe(mosq, NULL, "test", 2);
	else
		fprintf(stderr, "Сбой подключения\n");
}

/* Вывод сообщения об успешном подключении */
void SubscribeCallback(struct mosquitto* mosq, void* userdata, int mid, int qos_count, const int* granted_qos)
{
	printf("Подключение прошло успешно\n");
}

int MosqConnect()
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
	mosquitto_connect_callback_set(mosq, ConnectCallback);
	mosquitto_message_callback_set(mosq, MessageCallback);
	mosquitto_subscribe_callback_set(mosq, SubscribeCallback);

	if (mosquitto_connect(mosq, host, port, keepalive)) {
		/* Вывод ошибки о неудачной попытке подключения */
		fprintf(stderr, "Не удается подключиться.\n");
		return 1;
	}

	while (!mosquitto_loop(mosq, -1, 1)) {
	}
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}