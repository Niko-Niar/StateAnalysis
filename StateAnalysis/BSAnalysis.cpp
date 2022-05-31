#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

void analiz(vector<double>& x, bool& chek)
{
    double sum = 0;
    double average = 0;
    int size = x.size();// кол-во элементов в массиве

    for (int i = 0; i < x.size(); i++)
    {
        cout << x[i] << endl;
        sum = sum + x[i];
    }
    average = sum / x.size();
    cout << "Среднее значение :" << average << endl;

    //Среднеквадратичное отклонение
    sum = 0;
    for (int i = 0; i < x.size(); i++)
    {
        double a = x[i] - average;
        double aa = a * a;
        sum = sum + aa;
    }

    sum = sum / size;
    double sigma = sqrt(sum);
    cout << sigma;

    for (int i = 0; i < x.size(); i++)
    {
        if (x[i] >= average + sigma)
        {

            cout << " слабый выброс : ";
            cout << x[i] << endl;

        }

        if (x[i] >= average + sigma * 2)
        {
            cout << " средний выброс :";
            cout << x[i] << endl;


        }

        if ((x[i] >= average + sigma * 3))
        {
            cout << " сильный выброс :";
            cout << x[i] << endl;


        }

    }
    if (bool chek = true)
    {
        chek = false;
    }
    else chek = true;

}

void new_vektor(vector<double>& x, vector<double>& x_new)
{
    for (int i = 0; i < x.size(); i++)
    {
        x[i] = (x[i] + x_new[i]) / 2;

    }

}

void check(vector<double>& x, bool& chek)
{
    vector<double> x_1;
    if (chek = true) {

    }
    else {
        for (int i = x.size(); i != -1; i--)
        {
            x_1[i] = x[i];
        }

        for (int i = 0; i < x.size(); i++)
        {
            x[i] = x_1[i];
        }
    }

}

void main1(vector<float> v)
{
    setlocale(LC_ALL, "ru");
    bool chek = true;
    vector<double> x;
    for (int i = 0; i < v.size(); i++)
    {
        x.push_back(double(v[i]));
    }
    /*x.push_back(170);
    x.push_back(175);
    x.push_back(167);
    x.push_back(169);
    x.push_back(173);
    x.push_back(176);*/


    /*vector<int> y;
    y.push_back(5000);
    y.push_back(4000);
    y.push_back(3000);
    y.push_back(700);
    y.push_back(432);
    y.push_back(1234);*/

    analiz(x, chek);



    /*vector<double> x_new;*/
    /*x_new.push_back(177);
    x_new.push_back(171);
    x_new.push_back(165);
    x_new.push_back(165);
    x_new.push_back(179);
    x_new.push_back(171);*/


    /*vector<int> y_new;
    y_new.push_back(5600);
    y_new.push_back(4040);
    y_new.push_back(3010);
    y_new.push_back(703);
    y_new.push_back(436);
    y_new.push_back(1235);*/

    /*new_vektor(x, x_new);
    check(x, chek);
    analiz(x, chek);*/


}

vector<float> Filtr(vector<float> in, vector<float> out) {
    if (in.size() % 3 == 0) {
        for (int i = 3; i < in.size(); i += 3) {
            out.push_back((in[i - 3] + in[i - 2] + in[i - 1]) / 3);
        }
    }
    else {
        int n = in.size() - (in.size() / 3) * 3;
        for (int i = 3; i < in.size(); i += 3) {
            out.push_back((in[i - 3] + in[i - 2] + in[i - 1]) / 3);
        }
        int check = 0;
        float h = 0;
        for (int i = in.size() - n; i < in.size(); i++) {
            check++;
            h += in[i];
        }
        out.push_back(h / check);
    }
    return out;
}

void Interpolation(vector<float> v_1, vector<float> v_2)
{
    float del;
    vector<float> m_1 = v_1;
    vector<float> m_2 = v_2;
    vector<float> filt_m_1;
    vector<float> filt_m_2;
    vector<int> y_1;
    vector<int> y_2;
    vector<float> result;

    filt_m_1 = Filtr(m_1, filt_m_1);
    filt_m_2 = Filtr(m_2, filt_m_2);

    int size_1 = filt_m_1.size();
    int size_2 = filt_m_2.size();


    if (size_2 > size_1) {
        y_1.push_back(1);

        if (size_2 % size_1 > 4)
            del = size_2 / size_1 + 1;
        else del = size_2 / size_1;

        int check = 1;

        for (int i = del; i < size_2 + 1; ) {
            if (check == size_1 - 1)
                y_1.push_back(size_2);
            y_1.push_back(i + 1);
            i += del;
            check++;
        }

        for (int i = 1; i < size_2 + 1; i++) {
            y_2.push_back(i);
        }

        int counter = 0;
        result.push_back(filt_m_1[0]);
        for (int i = 1; i < size_2; i++) {
            int up_dist = y_2[i];
            int down_dist = y_1[counter + 1];
            if (down_dist > up_dist)
                result.push_back(filt_m_1[counter] + (filt_m_1[counter + 1] - filt_m_1[counter]) / (y_1[counter + 1] - y_1[counter]) * (y_2[i] - y_1[counter]));
            else
                result.push_back(filt_m_1[counter + 1]);

            if (y_2[i] >= y_1[counter + 1])
                counter++;
        }

        /*for (int i = 0; i < size_1; i++) {
            cout << y_1[i];
        }
        for (int i = 0; i < size_1; i++) {
            cout << y_2[i];
        }*/

        /*for (int i = 0; i < result.size(); i++) {
            cout << result[i] << "\n";
        }*/
        main1(result);
    }

}