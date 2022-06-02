#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

/* Позиции дефектов */
vector<int> error_number1, error_number2;

/* Значения дефектов (не использовал) */
vector<float> error_meaning1, error_meaning2;

/* Если дефекты идут подряд, то обрезаем */
void Cut(int begin, int end)
{
    int first = begin, second = end;
    for (int i = second; i < error_number2.size(); i++)
    {
        if (error_number2[i] - error_number2[i - 1] == 1)
            second = i;
        else
            break;
    }
    cout << "Дефект c началом на позиции: " << error_number2[first] << " и концом на позиции: " << error_number2[second] << endl;
    error_number2.erase(error_number2.cbegin() + first + 1, error_number2.cbegin() + second + 1);
    error_meaning2.erase(error_meaning2.cbegin() + first + 1, error_meaning2.cbegin() + second + 1);
}

/* Определяем уровень дефекта */
/* num используется для записи в разные массивы, если true - в первый, если false - во второй (не используется) */
void Identify(vector<float> x, bool num)
{
    float sum = 0;
    float average = 0;
    vector<int> number;
    vector<float> meaning;

    for (int i = 0; i < x.size(); i++)
        sum = sum + x[i];

    average = sum / x.size();
    cout << "Среднее значение :" << average << endl;

    //Среднеквадратичное отклонение
    sum = 0;
    for (int i = 0; i < x.size(); i++)
    {
        float a = x[i] - average;
        float aa = a * a;
        sum = sum + aa;
    }

    sum = sum / x.size();
    float sigma = sqrt(sum);
    cout << "Среднеквадратичное отклонение " << sigma << endl;

    for (int i = 0; i < x.size(); i++)
    {
        if (x[i] >= average + sigma && x[i] <= average + sigma * 2)
        {

            cout << "Cлабый дефект : " << x[i] << endl;
            number.push_back(i);
            meaning.push_back(x[i]);
        }

        if (x[i] >= average + sigma * 2 && x[i] <= average + sigma * 3)
        {
            cout << "Cредний дефект :" << x[i] << endl;
            number.push_back(i);
            meaning.push_back(x[i]);
        }

        if ((x[i] >= average + sigma * 3))
        {
            cout << "Cильный дефект :" << x[i] << endl;
            number.push_back(i);
            meaning.push_back(x[i]);
        }
    }

    if (num == true)
    {
        error_number1 = number;
        error_meaning1 = meaning;
    }
    else
    {
        error_number2 = number;
        error_meaning2 = meaning;
    }
    cout << "Количество дефектов: " << number.size() << endl;
}

/* Сглаживание значений */
vector<float> NewVektor(vector<float> x, vector<float> x_new)
{
    for (int i = 0; i < x.size(); i++)
        x[i] = (x[i] + x_new[i]) / 2;
    return x;
}

/* Переворот вектора */
vector<float> Check(vector<float>x)
{
    vector<float> x_1;
    for (int i = 0; i < x.size(); i++)
    {
        x_1.push_back(x[x.size() - i - 1]);
        }
    x.clear();
    for (int i = 0; i < x_1.size(); i++)
    {
        x.push_back(x_1[x_1.size() - i - 1]);
    }
    return x;
}

/* Определение типа ошибки */
void ErrorType()
{
    /* Дефект всего 1 */
    if (error_number2.size() == 1)
        cout << "Дефект на винте, позиция: " << error_number2[0] << endl;

    /* Проверяем нет ли дефектов подряд, если есть вырезаем */
    int interval = error_number2[1] - error_number2[0];
    if (interval == 1)
        Cut(0, 1);

    if (error_number2.size() == 1)
        cout << "Дефект на винте, позиция: " << error_number2[0] << endl;
    else
    {
        int x = -10; // погрешность 
        interval = error_number2[1] - error_number2[0];
        
        bool flag = true;
        int Check = 0;

        /* Проверяем на одинаковое растояние между дефектами */
        for (int i = 1; i < error_number2.size() - 1; i++)
        {
            if (error_number2[i + 1] - error_number2[i] == 1)
                Cut(i, i + 1);
            if (i == error_number2.size() - 1)
                break;
            else
            {
                for (x = -10; x < 11; x++)
                {
                    int n1 = error_number2[i];
                    int n2 = error_number2[i + 1] - interval;
                    if (error_number2[i] == error_number2[i + 1] - interval + x)
                    {
                        cout << "Одинаковый промежуток между дефектами " << i + 1 << " и " << i + 2 << endl;
                        Check++;
                        break;
                    }
                }
                if (Check == 0)
                {
                    flag = false;
                    break;
                }
                Check = 0;
            }
        }
        
        /* Если расстояние одинаковое, то дефект на гайке */
        if (flag == true)
        {
            cout << "Дефекты на гайке, на позициях: ";
            for (int i = 0; i < error_number2.size(); i++)
            {
                cout << error_number2[i] << ", ";
            }
            cout << endl;
        }
        /* Если расстояние разное, то дефект на винте */
        else
        {
            cout << "Дефекты на винте, на позициях: ";
            for (int i = 0; i < error_number2.size(); i++)
            {
                cout << error_number2[i] << " ";
            }
            cout << endl;
        }
    }
}

void Analysis(vector<float> result, int num)
{
    int n = num;
    setlocale(LC_ALL, "ru");
    vector<float> x = result, x_old;

    /* Если первый прогон, то создаем файлик в который будем записывать полученные значения */
    if (n == 1)
    {
        x_old = x;

        ofstream fout("lcs.txt");
        if (fout.is_open())
            for (int i = 0; i < x.size(); i++)
                fout << x[i] << " ";
        fout.close();
    }
    /* Если не первый, то считываем */
    else
    {
        ifstream in("lcs.txt");
        string number;
        if (in.is_open())
        {
            while (getline(in, number, ' '))
            {
                x_old.push_back(atoi(number.c_str()));
                /*printf("%d\n", atoi(number.c_str()));*/
            }
        }
        in.close();
    }

    /* Если четный прогон, то отправляем на разворот */
    if (n % 2 == 0)
        x = Check(x);;

    x = NewVektor(x, x_old);

    ofstream fout("lcs.txt");
    if (fout.is_open())
        for (int i = 0; i < x.size(); i++)
            fout << x[i] << " ";
    fout.close();

    //Identify(x_old, true);
    Identify(x, false);
    
    if (error_number2.size() == 0)
        cout << "Дефекты не обнаружены" << endl;
    else
        ErrorType();
}