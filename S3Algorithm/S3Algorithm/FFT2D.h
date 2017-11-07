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
	FFT2D( 
		__in UINT size 
		);

	~FFT2D( void );

	void Process( 
		__in Image* image, 
		__in int baseINdex 
		);

	Complex **m_ppData;

private:
	inline void FFT( void );

	double *m_pReal;
	double *m_pImag;
	int size;
	int logSize;
};

