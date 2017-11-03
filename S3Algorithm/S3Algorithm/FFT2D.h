#pragma once
#include "stdafx.h"

struct Complex
{
	double Real, Imaginary;
};
class FFT2D
{
public:
	FFT2D( int size );
	~FFT2D( void );
	void Process( Complex **data );
	double log2;
	Complex **complex;

private:
	inline void FFT( double *x, double *y );
	void ShiftData( Complex **data );
	double *real;
	double *imag;
	int size;
	int logSize;
};

