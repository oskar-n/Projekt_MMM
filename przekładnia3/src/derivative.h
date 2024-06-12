#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <conio.h>




struct parameters {

	double n1, n2, ke, kt, R1, L1, J, J1, J2, i1;
	parameters() {
		n1 = 1; n2 = 1; ke = 1, kt = 1, R1 = 1, L1 = 1, J = 1, J1 = 1, J2 = 1, i1 = 1;
	}
};


//obliczanie pochodnych funkcji I i W
void counting(parameters param, std::vector<double>& us, std::vector<double>& I, std::vector<double>& W, double total, float krok)
{

	std::vector<double> i1p, w1p; //pochodne funkcji I i W
	i1p.resize(total); w1p.resize(total);
	

	i1p[0] = 0; w1p[0] = 0; //zerowe warunki poczatkowe

	for (int i = 0; i < total - 1; i++)
	{
		
		i1p[i + 1] =  -(param.R1 / param.L1) * I[i] - (param.ke / param.L1) * W[i] + (us[i] / param.L1); //pierwsza pochodna I

		w1p[i + 1] =  (param.kt / param.J) * I[i];//pierwsza pochodna W
		
		I[i + 1] = I[i] + krok * i1p[i];//rownania na I

		W[i + 1] = W[i] + krok * w1p[i];//rownania na W

	}

	for (int i = 0; i < total-1; i++)
	{
		W[i] = W[i] * 1/param.i1; //przeliczenie na predkosc katowa
	}

}


void create_param(parameters& param) {
	param.i1 = param.n2 / param.n1;
	param.J = param.J1 + (param.J2 * 1 / (param.i1 * param.i1));
}
