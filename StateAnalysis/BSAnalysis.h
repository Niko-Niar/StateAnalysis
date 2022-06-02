#ifndef BSANALYSIS_H
#define BSANALYSIS_H

#include <vector>

using namespace std;

void Cut(int begin, int end);

void Identify(vector<float> x, bool num);

vector<float> NewVektor(vector<float> x, vector<float> x_new);

vector<float> Check(vector<float> x);

void ErrorType();

void Analysis(vector<float> result, int num);

#endif