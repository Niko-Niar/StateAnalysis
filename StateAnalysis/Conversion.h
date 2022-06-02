#ifndef CONVERSION_H
#define CONVERSION_H

#include <vector>

using namespace std;

vector<float> Filtr(vector<float> in);

vector<float> Interp(int del, int size_1, int size_2, vector<float> filt_m_1, vector<float> result);

vector<float> Resize(vector<float> m_1, vector<float> m_2);

vector<float> Conversion(vector<float> v_1, vector<float> v_2);

#endif
