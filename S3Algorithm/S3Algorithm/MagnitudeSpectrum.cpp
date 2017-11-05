#include "stdafx.h"
#include "MagnitudeSpectrum.h"


MagnitudeSpectrum::MagnitudeSpectrum( UINT blockSize, UINT fourierReduction,  bool skin )
{
	m_pRadialFrequency = new RadialFrequency( blockSize, blockSize / 2 );
	m_pFourier = new FFT2D( blockSize );
	m_pLinearRegresion = new LinearRegression();
	this->m_FourierReduction = fourierReduction;
	this->m_Skin = skin;
}
