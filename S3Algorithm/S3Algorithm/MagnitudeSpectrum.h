#pragma once
#include "LinearRegression.h"
#include "FFT2D.h"
#include "RadialFrequency.h"


class MagnitudeSpectrum
{
public:
	MagnitudeSpectrum( UINT blockSize, UINT fourierReduction,  bool skin );


	RadialFrequency *m_pRadialFrequency;
	FFT2D *m_pFourier;
	LinearRegression *m_pLinearRegresion;
	UINT m_FourierReduction;
	bool m_Skin;
};

