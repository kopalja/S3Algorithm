#include "stdafx.h"
#include "LinearRegression.h"
#include <iostream>
#include <math.h>
using namespace std;

double LinearRegression::GetAlpha(double* input, int length)
{
	double RightHandSide[2];
	RightHandSide[0] = 0;
	RightHandSide[1] = 0;
	for (int i = 0; i < length; i++)
	{
		RightHandSide[0] += i * input[i];
		RightHandSide[1] += input[i];
	}

	double LeftCol[2];
	double RightCol[2];

	double sum = 0;
	RightCol[1] = length;
	sum = (length - 1) * (1 + (length - 1)) / 2;
	LeftCol[1] = sum;
	RightCol[0] = sum;
	sum = (2 * (pow((length - 1), 3)) + 3 * (pow((length - 1), 2)) + (length - 1)) / 6;
	LeftCol[0] = sum;

	double koef = RightCol[0] / LeftCol[0];
	LeftCol[0] *= koef;
	LeftCol[1] *= koef;
	RightHandSide[0] *= koef;

	RightCol[0] -= LeftCol[0];
	RightCol[1] -= LeftCol[1];
	RightHandSide[1] -= RightHandSide[0];

	koef = RightCol[1];
	RightCol[1] /= koef;
	RightHandSide[1] /= koef;

	koef = LeftCol[0];
	RightHandSide[0] /= koef;
	LeftCol[0] /= koef;
	LeftCol[1] /= koef;

	koef = RightCol[1] / LeftCol[1];
	RightHandSide[0] -= RightHandSide[1] / koef;

	return RightHandSide[0];
}


LinearRegression::LinearRegression()
{
}


LinearRegression::~LinearRegression()
{
}