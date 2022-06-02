#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

/* ‘ильтр дл€ удалени€ шумов */
vector<float> Filtr(vector<float> in) {
    vector<float> out;
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
        int Check = 0;
        float h = 0;
        for (int i = in.size() - n; i < in.size(); i++) {
            Check++;
            h += in[i];
        }
        out.push_back(h / Check);
    }
    return out;
}

/* »нтерпол€ци€ (увеличение количества данных путем предсказывани€) */
vector<float> Interp(int del, int size_1, int size_2, vector<float> filt_m_1, vector<float> result) {
    vector<int> y_1;
    vector<int> y_2;
    y_1.push_back(1);

    int Check = 1;

    for (int i = del; i < size_2 + 1; ) {
        if (Check == size_1 - 1)
            y_1.push_back(size_2);
        y_1.push_back(i + 1);
        i += del;
        Check++;
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
    return result;
}

/* »зменение данных в зависимости от скорости движени€ */
vector<float> Resize(vector<float> m_1, vector<float> m_2) {
    int del;
    vector<float> piece;
    vector<float> result;
    vector<float> dop;

    int j = 10;
    for (int i = 0; i < m_1.size(); i++) {
        if (m_1[i] == 0)
            j += 10;
        if (m_1[i] == 1000) {
            for (int k = j; k < j + 10; k++) {
                result.push_back(m_2[k]);
            }
            j += 10;
        }

        if (m_1[i] > 1000) {
            dop.clear();
            piece.clear();
            if (int(m_1[i]) % 1000 > 4)
                del = m_1[i] / 1000 + 1;
            else del = m_1[i] / 1000;

            int size_1 = 10;
            int size_2 = 10 * del;

            for (int k = j; k < j + 10; k++) {
                piece.push_back(m_2[k]);
            }

            dop = Interp(del, size_1, size_2, piece, dop);

            for (int i = 0; i < dop.size(); i++) {
                result.push_back(dop[i]);
            }
            j += 10;
        }

        if (m_1[i] < 1000 && m_1[i] != 0) {
            if (1000 % int(m_1[i]) > 4)
                del = 1000 / m_1[i] + 1;
            del = 1000 / m_1[i];

            for (int k = j; k < j + 10; ) {
                result.push_back(m_2[k]);
                k += del;
            }
            j += 10;
        }
    }
    return Filtr(result);
}

vector<float> Conversion(vector<float> v_1, vector<float> v_2)
{
    vector<float> m_1 = v_1;
    vector<float> m_2 = v_2;
    vector<float> result;

    result.clear();
    result = Resize(m_1, m_2);

    for (int i = 0; i < result.size(); i++) {
        cout << result[i] << "\n";
    }

    return result;
}