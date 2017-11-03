#include "stdafx.h"
#include <stdlib.h> 
#include "FFT2D.h"
#include <math.h>

#include <iostream>
using namespace std;

# define M_PI           3.14159265358979323846  /* pi */


FFT2D::FFT2D( int size )
{
	real = new double[size];
	imag = new double[size];
	complex = new Complex*[size];
	for (int i = 0; i < size; i++)
	{
		complex[i] = new Complex[size];
	}

	this->size = size;
	log2 = log( 2.0 );
	logSize = log( size ) / log2;
}

FFT2D::~FFT2D( void )
{
	delete[] real;
	delete[] imag;
}

void FFT2D::Process( Complex **data )
{
	int i,j;
	int twopm;
	/* Transform the rows */

	for (i=0;i<size;i++) 
	{
		for (j=0; j<size;j++) 
		{
			real[j] = data[j][i].Real;
			imag[j] = data[j][i].Imaginary;
		}
		FFT( real, imag );
		for (j=0;j<size;j++) 
		{
			data[j][i].Real = real[j];
			data[j][i].Imaginary = imag[j];
		}
	}

	 /* Transform the columns */

	for (i=0;i<size;i++) 
	{
		for (j=0;j<size;j++) 
		{
			real[j] = data[i][j].Real;
			imag[j] = data[i][j].Imaginary;
		}
		FFT( real, imag );
		for (j=0;j<size;j++) 
		{
			data[i][j].Real = real[j];
			data[i][j].Imaginary = imag[j];
		}
	}
	ShiftData( _Inout_ data );
}


inline void FFT2D::FFT( double *x,double *y )
{
	long nn,i,i1,j,k,i2,l,l1,l2;
	double c1,c2,tx,ty,t1,t2,u1,u2,z;

	/* Calculate the number of points */
	nn = size;

	/* Do the bit reversal */
	i2 = nn >> 1;
	j = 0;
	for (i=0;i<nn-1;i++) 
	{
		if (i < j) 
		{
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
		}
		k = i2;
		while (k <= j) 
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
	for (l=0;l<logSize;l++) 
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j=0;j<l1;j++) 
		{
			for (i=j;i<nn;i+=l2)
			{
				i1 = i + l1;
				t1 = u1 * x[i1] - u2 * y[i1];
				t2 = u1 * y[i1] + u2 * x[i1];
				x[i1] = x[i] - t1;
				y[i1] = y[i] - t2;
				x[i] += t1;
				y[i] += t2;
			}
			z =  u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
        c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for forward transform */
    for (i=0;i<nn;i++) 
	{
		x[i] /= (double)nn;
        y[i] /= (double)nn;
	}
}

void FFT2D::ShiftData( _Inout_ Complex **complex )
{
	Complex tmp;
	int halfSize = size >> 1;
	for (int y = 0; y < halfSize; y++)
	{
		for (int x = 0; x < halfSize; x++)
		{
			tmp = complex[x][y];
			complex[x][y] = complex[x + halfSize][y + halfSize];
			complex[x + halfSize][y + halfSize] = tmp;

			tmp = complex[x + halfSize][y];
			complex[x + halfSize][y] = complex[x][y + halfSize];
			complex[x][y + halfSize] = tmp;
		}
	}
}