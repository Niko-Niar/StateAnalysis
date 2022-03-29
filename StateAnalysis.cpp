//#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

int main() {
	setlocale(LC_ALL, "Russian");
	//ifstream inputFile("C:/1/1618833245_100000.log");
	ifstream inputFile("C:/1/1.txt");
	string str, s;
	int n = 0;
	int g = 0;
	int b = 0;
	int compare = 26267;
	while (getline(inputFile, str)) // считываем кол-во чисел в файле
	{
		str += ' ';
		for (int i = 0; i < str.length(); i++)
		{
			if (isdigit(str[i]) || str[i] == '.')
				s += str[i];
			if (!(isdigit(str[i])) && str[i] != '.')
			{
				if (s != "")
					n++;
				s = "";
			}
		}
	}
	istringstream in(str);
	float* mass = new float[(int)n]; // создаём массив
	for (int i = 0; i < n; ++i) // заполняем массив числами из файла
	{
		in >> mass[i];
	}
	cout << "mass = ";
	for (int i = 0; i < n; ++i) // выводим массив, сравниваем значения
	{
		cout << mass[i] << " ";
		if (mass[i] > compare)
			g++;
		else b++;
	}
	cout << endl;
	cout << "All = " << n << endl;
	cout << "Good (> " << compare << ") = " << g << endl;
	cout << "Bad (<= " << compare << ") = " << b << endl;
	if (b == 0)
		cout << "Result = Неисправностей не обнаружено" << endl;
	else
		cout << "Result = Обнаружены неисправности" << endl;
	inputFile.close();
	return 0;
}