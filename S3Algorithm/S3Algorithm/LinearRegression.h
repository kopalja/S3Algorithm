#pragma once


class LinearRegression
{
public:
	double GetAlpha( double* input, int length );
	
	LinearRegression();
	~LinearRegression();

private:

	double* m_SumOfSquares;
	double* m_SumOfIntegers;
	double* m_NormCoefficient;
	double* m_SquareCoefficient;

};

