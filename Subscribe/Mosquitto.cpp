#pragma comment(lib, "mosquitto.lib")

#include <stdio.h>
#include <mosquitto.h>
#include <iostream>

void my_message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
	if (message->payloadlen) {
		printf("%s\n", message->payload); // Вывод полученного сообщения
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

int main(int argc, char* argv[])
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