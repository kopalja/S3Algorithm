#pragma once

#include "RadialFrequency.h"
#include "LinearRegression.h"
#include "FFT2D.h"



class MagnitudeSpectrum
{
public:
	MagnitudeSpectrum( __in UINT blockSize, __in UINT fourierReduction,  __in bool skin );
	~MagnitudeSpectrum( void );


	BYTE GetSlope( 
		__in UINT baseIndex,
		__in Image *pGrayImage
		);

	RadialFrequency *m_pRadialFrequency;
	FFT2D *m_pFourier;
	LinearRegression *m_pLinearRegresion;
	UINT m_FourierReduction;
	bool m_Skin;

	double m_Log2; 

private:
	UINT blockSize;
	UINT vectorLength;
};

