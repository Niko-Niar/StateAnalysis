#ifndef BSANALYSIS_H
#define BSANALYSIS_H

#include <vector>

using namespace std;

vector<float> Filtr(vector<float> in, vector<float> out);

void Interpolation(vector<float> v_1, vector<float> v_2);

void analiz(vector<float>& x, bool& chek);

void new_vektor(vector<float>& x, vector<float>& x_new);

void check(vector<float>& x, bool& chek);

#endif