#include "stdafx.h"
#include <stdlib.h> 
#include "FFT2D.h"
#include <math.h>

#include <iostream>
using namespace std;

# define M_PI           3.14159265358979323846  /* pi */


FFT2D::FFT2D( UINT size )
{
	m_pReal = new double[size];
	m_pImag = new double[size];
	m_ppData = new Complex*[size];
	for (int i = 0; i < size; i++)
	{
		m_ppData[i] = new Complex[size];
	}

	this->size = size;
	logSize = log( size ) / log( 2.0 );
}

FFT2D::~FFT2D( void )
{
	delete[] m_pReal;
	delete[] m_pImag;
	for (int i = 0; i < size; i++)
	{
		delete[] m_ppData[i];
	}
	delete[] m_ppData;
}

void FFT2D::Process( Image* image, int baseIndex  )
{
	int i,j;
	int twopm;

	/* Transform the rows */
	for ( i = 0; i < size; i++ ) 
	{
		for ( j = 0; j < size; j++ ) 
		{ 
			m_pReal[j] = image->buffer[ baseIndex + i * image->width + j ];
			m_pImag[j] = 0;
		}
		FFT();
		for ( j = 0; j < size; j++ ) 
		{
			m_ppData[j][i].Real = m_pReal[j];
			m_ppData[j][i].Imaginary = m_pImag[j];
		}
	}

	/* Transform the columns */
	for ( i = 0; i < size; i++ ) 
	{
		for ( j = 0; j < size; j++ ) 
		{
			m_pReal[j] = m_ppData[i][j].Real;
			m_pImag[j] = m_ppData[i][j].Imaginary;
		}
		FFT();
		for ( j = 0; j < size; j++ ) 
		{
			m_ppData[i][j].Real = m_pReal[j];
			m_ppData[i][j].Imaginary = m_pImag[j];
		}
	}
}


inline void FFT2D::FFT( void )
{  
	UINT i,i1,j,k,i2,l,l1,l2;
	double c1,c2,tx,ty,t1,t2,u1,u2,z;


	/* Do the bit reversal */
	i2 = size >> 1;
	j = 0;
	int nnMinusOne = size - 1;
	for ( i = 0; i < nnMinusOne; i++ ) 
	{
		if (i < j) 
		{
			tx = m_pReal[i];
			ty = m_pImag[i];
			m_pReal[i] = m_pReal[j];
			m_pImag[i] = m_pImag[j];
			m_pReal[j] = tx;
			m_pImag[j] = ty;
		}
		k = i2;
		while ( k <= j ) 
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for ( l = 0; l < logSize; l++ ) 
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for ( j=0; j<l1; j++ ) 
		{
			for ( i = j; i < size; i += l2 )
			{
				i1 = i + l1;
				t1 = u1 * m_pReal[i1] - u2 * m_pImag[i1];
				t2 = u1 * m_pImag[i1] + u2 * m_pReal[i1];
				m_pReal[i1] = m_pReal[i] - t1;
				m_pImag[i1] = m_pImag[i] - t2;
				m_pReal[i] += t1;
				m_pImag[i] += t2;
			}
			z =  u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt( ( 1.0 - c1 ) / 2.0 );
        c2 = -c2;
		c1 = sqrt( ( 1.0 + c1 ) / 2.0 );
	}

	/* Scaling for forward transform */
    for ( i = 0; i < size; i++ ) 
	{
		m_pReal[i] /= (float)size;
        m_pImag[i] /= (float)size;
	}
}