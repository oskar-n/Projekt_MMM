#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <conio.h>


// deklaracje zasiêgów
using namespace std;
// sta³e w programie
#define N 4 // rz¹d systemu
#define h 0.001 // krok obliczeñ
#define T 10.0 // ca³kowity czas symulacji – przedzia³ [0 , T]
#define L 2.5 // liczba okresów sygna³u sinus w przedziale T
#define M 8.0 // amplituda sygna³u sinus
#define PI 3.14159265 // liczba PI
// nowe typy – macierz kwadratowa (NxN) i wektor (Nx1)
typedef struct { double n[N]; } Vect;
typedef struct { double n[N][N]; } Matr;
// pomocniczy typ – kolejne bajty danej ’double’
typedef union { char c[sizeof(double)]; double d; } Box;
// zmienne globalne w programie
double us[(int)(1.0 * T / h) + 1]; // sygna³ wejœciowy sinus
double uf[(int)(1.0 * T / h) + 1]; // sygna³ wejœciowy fala prostok¹tna
double y[(int)(1.0 * T / h) + 1]; // sygna³ wyjœciowy
Box z; // zmienna: pojedyncza wartoœæ sygna³u (u lub y)
// dodawanie wektorów - operator przeci¹¿ony
Vect operator+(const Vect& U, const Vect& V)
{
	Vect W;
	int i;
	for (i = 0; i < N; i++)
		W.n[i] = U.n[i] + V.n[i];
	return W;
}
// mno¿enie: wektor razy skalar - operator przeci¹¿ony
Vect operator*(const Vect& U, const double& d)
{
	Vect W;
	int i;
	for (i = 0; i < N; i++)
		W.n[i] = U.n[i] * d;
	return W;
}
// mno¿enie: macierz razy wektor - operator przeci¹¿ony
Vect operator*(const Matr& A, const Vect& V)
{
	Vect W;
	int i, j;
	for (i = 0; i < N; i++)
		for (j = 0, W.n[i] = 0; j < N; j++)
			W.n[i] += A.n[i][j] * V.n[j];
	return W;
}
// mno¿enie skalarne: wektor razy wektor - operator przeci¹¿ony
double operator*(const Vect& U, const Vect& V)
{
	int i;
	double s;
	for (i = 0, s = 0; i < N; i++)
		s += U.n[i] * V.n[i];
	return s;
}


struct parameters {

    double n1, n2, ke, kt, R1, L1, J, J1, J2, i1;
    parameters() {
        n1 = 1; n2 = 1; ke = 1, kt = 1, R1 = 1, L1 = 1, J = 1, J1 = 1, J2 = 1, i1 = 1;
    }
};

void counting( parameters);

void create_param(parameters& );


void counting(parameters param)
{
	//int i, total; // zmienne pomocnicze
	//double w; // czêstotliwoœæ sygna³u wejœciowego
	//total = sizeof(u) / sizeof(u[0]); // rozmiar wektorów danych
	//w = 2.0 * PI * L / T; // czêstotliwoœæ sinusoidy
	//for (int i = 0; i < total; i++) // sygna³ wejœciowy i jego pochodne
	//{
	//	u[i] = M * sin(w * i * h); // sygna³ wejœciowy: u=M*sin(w*t) , t=i*h
	//	u1p[i] = M * w * cos(w * i * h); // pochodna 1: u’(t)
	//	u2p[i] = -M * w * w * sin(w * i * h); // pochodna 2: u’’(t)
	//	u3p[i] = -M * w * w * w * cos(w * i * h); // pochodna 3: u’’’(t)
	//}
	//y[0] = 0; y1p[0] = 0; y2p[0] = 0; y3p[0] = 0; // zerowe warunki pocz¹tkowe
	//for (i = 0; i < total - 1; i++) // g³ówna pêtla obliczeñ
	//{
	//	y4p[i] = -0 * y3p[i] - 1 * y2p[i] - param.R1 / param.L1 * param.kt / param.J * y1p[i] - param.ke * param.kt / (param.J * param.L1) * y[i] + 0 * u3p[i] + 0 * u2p[i] + 0 * u1p[i] + param.kt / (param.J * param.L1) * u[i];
	//	y3p[i + 1] = y3p[i] + h * y4p[i];
	//	y2p[i + 1] = y2p[i] + h * y3p[i] + (h * h / 2.0) * y4p[i];
	//	y1p[i + 1] = y1p[i] + h * y2p[i] + (h * h / 2.0) * y3p[i] + (h * h * h / 6.0) * y4p[i];
	//	y[i + 1] = y[i] + h * y1p[i] + (h * h / 2.0) * y2p[i] + (h * h * h / 6.0) * y3p[i] + (h * h * h * h / 24.0) * y4p[i];
	//}
	// zapisanie macierzy i wektorów modelu
	A.n[0][0] = 0; A.n[0][1] = 1; A.n[0][2] = 0; A.n[0][3] = 0;
	A.n[1][0] = 0; A.n[1][1] = 0; A.n[1][2] = 1; A.n[1][3] = 0;
	A.n[2][0] = 0; A.n[2][1] = 0; A.n[2][2] = 0; A.n[2][3] = 1;
	A.n[3][0] = -a0; A.n[3][1] = -a1; A.n[3][2] = -a2; A.n[3][3] = -a3;
	B.n[0] = 0; B.n[1] = 0; B.n[2] = 0; B.n[3] = 1;
	C.n[0] = b0; C.n[1] = b1; C.n[2] = b2; C.n[3] = b3;
	D = 0;
	total = sizeof(us) / sizeof(us[0]); // rozmiar wektorów danych
	w = 2.0 * PI * L / T; // czêstotliwoœæ sinusoidy
	for (i = 0; i < total; i++) // obliczenie pobudzenia – sinus lub fala prostok¹tna
	{
		us[i] = M * sin(w * i * h); // sygna³ wejœciowy sinus: u=M*sin(w*t) , t=i*h
		uf[i] = ((us[i] > 0) ? M : -M); // sygna³ wejœciowy fala prostok¹tna
	}
	// zerowe warunki pocz¹tkowe
	xi_1.n[0] = 0; xi_1.n[1] = 0; xi_1.n[2] = 0; xi_1.n[3] = 0;
	// g³ówna pêtla obliczeñ - zamiast pobudzenia sinus (us) mo¿na wstawiæ falê (uf)
	for (i = 0; i < total; i++)
	{
		Ax = A * xi_1; Bu = B * us[i]; Cx = C * xi_1; Du = D * us[i];
		xi = Ax + Bu; xi = xi * h; xi = xi_1 + xi; xi_1 = xi; y[i] = Cx + Du;
		y[i]=1 / param.i1 * y[i]
	}
}

void create_param(parameters& param) {
	param.i1 = param.n1 / param.n2;
	param.J = param.J1 + param.J2 * 1 / (param.i1 * param.i1);
}
