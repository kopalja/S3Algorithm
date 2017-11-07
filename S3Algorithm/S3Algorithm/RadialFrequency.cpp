#include "stdafx.h"
#include "RadialFrequency.h"
#include <math.h>

/*
	Pre-count distance from the middle for all points in block. (distance[x][y])
	Create and count x-coordinate for all differente distances of pointes from the middle. (resultFunc).
	Y-coordiantes has to be counted later.
*/

RadialFrequency::RadialFrequency( __in int blockSize, __in int resolution )
{
	m_BlockSize = blockSize;

	/* Aloc meber variables, dealoc in decstuctor */
	m_pResultFunction = new double[resolution];
	m_ppDistance = new BYTE*[blockSize];
	for (int i = 0; i < blockSize; i++)
		m_ppDistance[i] = new BYTE[blockSize];



	/* Count Real Distances from cester */
	double **ppRealDistance = new double*[blockSize];
	for (int i = 0; i < blockSize; i++)
		ppRealDistance[i] = new double[blockSize];
	CountRealDistances( ppRealDistance );


	/* Find all different dic=stances */
	double *pDifferentValues = new double[blockSize * blockSize];
	int numberOfDifferentDistances = FindDifferentDistances( ppRealDistance, pDifferentValues );

	/* Sort different distances */
	double *pSortedValues = new double[numberOfDifferentDistances];
	SortDifferentDistances( numberOfDifferentDistances, pDifferentValues, pSortedValues );


	/* Set member variables */
	SetIntDistances( numberOfDifferentDistances, resolution, ppRealDistance, pSortedValues );
	ShiftDistances();
		
	/* Dealoc */
	for (int i = 0; i < blockSize; i++) 
		delete[] ppRealDistance[i];
	delete[] ppRealDistance;
	delete[] pDifferentValues;
	delete[] pSortedValues;
}


RadialFrequency::~RadialFrequency( void )
{
	for (UINT i = 0; i < m_BlockSize; i++)
		delete[] m_ppDistance[i];
	delete[] m_ppDistance;
	delete[] m_pResultFunction;
}


void RadialFrequency::CountRealDistances(  __out double **ppRealDistance  )
{
	double halfSize = (double)m_BlockSize / (double)2;
	for (UINT y = 0; y < m_BlockSize; y++)
	{
		for (UINT x = 0; x < m_BlockSize; x++)
		{
			ppRealDistance[x][y] = sqrt( ( ( x - halfSize ) / halfSize ) * ( ( x - halfSize ) / halfSize ) + ( ( y - halfSize ) / halfSize ) * ( ( y - halfSize ) / halfSize ) );
		}
	}
}

int RadialFrequency::FindDifferentDistances( __in double **ppRealDistance, __out_ecount( m_BlockSize * m_BlockSize ) double *pDifferentDistance  )
{
	int difPointer = 0;
	for (UINT y = 0; y < m_BlockSize; y++)
	{
		for (UINT x = 0; x < m_BlockSize; x++)
		{
			for (int i = 0; i < difPointer + 1; i++)
			{
				if ( i == difPointer ) // if new distance isnot already in array;
				{
					pDifferentDistance[difPointer] = ppRealDistance[x][y];
					difPointer++;
					break;
				}
				if ( pDifferentDistance[i] == ppRealDistance[x][y] ) break;
			}
		}
	}
	return difPointer;
}

void RadialFrequency::SortDifferentDistances( __in UINT numberOfDifferentDistances, __in_ecount( m_BlockSize * m_BlockSize ) double *pDifferentDistances, __out_ecount( numberOfDifferentDistances )  double *pSortedDifferentDistances  )
{
	/* Sort different Values */
	for (UINT i = 0; i < numberOfDifferentDistances; i++)
	{
		for ( UINT j = 1; j < numberOfDifferentDistances; j++ )
		{
			if ( pDifferentDistances[j] < pDifferentDistances[j - 1] ) 
			{
				double temp = pDifferentDistances[j];
				pDifferentDistances[j] = pDifferentDistances[j - 1];
				pDifferentDistances[j - 1] = temp;
			}
		}
	}
	for (UINT i = 0; i < numberOfDifferentDistances; i++)
	{
		pSortedDifferentDistances[i] = pDifferentDistances[i];
	}
}


void RadialFrequency::SetIntDistances( __in UINT numberOfDifferentDistances, __in UINT resolution, __in double **ppRealDistance, __in_ecount( numberOfDifferentDistances ) double *pSortedDifferentDistances  )
{

	double highestValue = pSortedDifferentDistances [numberOfDifferentDistances - 1];

	for (UINT y = 0; y < m_BlockSize; y++)
	{
		for (UINT x = 0; x < m_BlockSize; x++)
		{
			for (UINT i = 0; i < numberOfDifferentDistances; i++)
			{
				if ( pSortedDifferentDistances[i] == ppRealDistance[x][y] )
				{
					m_ppDistance[x][y] = ppRealDistance[x][y] / highestValue * resolution;
					break;
				}
			}
		}
	}
}




void RadialFrequency::ShiftDistances()
{
	BYTE tmp;
	int halfSize = m_BlockSize / 2;
	for (int y = 0; y < halfSize; y++)
	{
		for (int x = 0; x < halfSize; x++)
		{
			tmp = m_ppDistance[x][y];
			m_ppDistance[x][y] = m_ppDistance[x + halfSize][y + halfSize];
			m_ppDistance[x + halfSize][y + halfSize] = tmp;

			tmp = m_ppDistance[x + halfSize][y];
			m_ppDistance[x + halfSize][y] = m_ppDistance[x][y + halfSize];
			m_ppDistance[x][y + halfSize] = tmp;
		}
	}
}