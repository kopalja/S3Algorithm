#include "stdafx.h"
#include "MagnitudeSpectrum.h"


MagnitudeSpectrum::MagnitudeSpectrum( UINT blockSize, UINT fourierReduction,  bool skin )
{
	m_pRadialFrequency = new RadialFrequency( blockSize, blockSize / 2 );
	m_pFourier = new FFT2D( blockSize );
	m_pLinearRegresion = new LinearRegression();

	this->m_FourierReduction = fourierReduction;
	this->m_Skin = skin;
	this->blockSize = blockSize;
	vectorLength = 4;
	m_Log2 = log( 2.0 );
}

MagnitudeSpectrum::~MagnitudeSpectrum( void )
{
	delete m_pRadialFrequency;
	delete m_pFourier;
	delete m_pLinearRegresion;
}


BYTE MagnitudeSpectrum::GetSlope( __in UINT baseIndex, __in Image *pGrayImage )
{
	m_pFourier->Process( pGrayImage, baseIndex );

	for (UINT i = 0; i < vectorLength; i++)
	{
		m_pRadialFrequency->m_pResultFunction[i] = 0;
	}

	for (UINT i = 0; i < blockSize; i++)
	{
		for (UINT j = 0; j < blockSize; j++)
		{
			if ( m_pRadialFrequency->m_ppDistance[j][i] < vectorLength )
				m_pRadialFrequency->m_pResultFunction[ m_pRadialFrequency->m_ppDistance[j][i] ] +=
				sqrt( m_pFourier->m_ppData[j][i].Real * m_pFourier->m_ppData[j][i].Real + m_pFourier->m_ppData[j][i].Imaginary * m_pFourier->m_ppData[j][i].Imaginary );
		}
	}
	for (UINT i = 0; i < vectorLength; i++)
	{
		m_pRadialFrequency->m_pResultFunction[i] = log( m_pRadialFrequency->m_pResultFunction[i] ) / m_Log2;
	}

	
	double alfa = -1 * m_pLinearRegresion->GetAlpha( m_pRadialFrequency->m_pResultFunction, vectorLength );

	if ( m_Skin )
	{
		if ( alfa < 1.25 ) return 255;
		else if ( alfa < 1.75 ) return 255.0 * 2.0 * ( 1.75 - alfa );
		else return 0; 
	}
	else
	{
		if ( alfa < 0.2 ) return 255;
		else if ( alfa < 1.2 ) return 255.0 * ( 1.2 - alfa );
		else return 0;
	}
}
