#pragma once
#include "stdafx.h"
#include "ImageHandler.h"

struct Complex
{
	double Real, Imaginary;
};
class FFT2D
{
public:
	FFT2D( int size );
	~FFT2D( void );
	void Process( Image* image, int baseINdex );
	double log2;
	Complex **data;

private:
	inline void FFT( double *x, double *y );
	void ShiftData( Complex **data );
	double *real;
	double *imag;
	int size;
	int logSize;
};

