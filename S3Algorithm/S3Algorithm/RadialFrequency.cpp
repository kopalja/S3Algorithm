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
	this->resolution = resolution;

	double **realDistance = CountRealDistances( __in blockSize );
	double *differentDistances = SortDifferentDistances( realDistance, blockSize );
	SetIntDistances( realDistance, differentDistances, blockSize );
	//CountOccurrences( blockSize );
		
	resultFunc = new double[resolution];
	for (int i = 0; i < resolution; i++) 
	{
		resultFunc[i] = 0;
	}

	for (int i = 0; i < blockSize; i++)
	{
		delete[] realDistance[i];
	}

	delete[] realDistance;
	delete[] differentDistances;
}


RadialFrequency::~RadialFrequency( void )
{
	delete[] resultFunc;
}


double** RadialFrequency::CountRealDistances( int size )
{
	double **realDistance = new double*[size];

	for (int i = 0; i < size; i++)
	{
		realDistance[i] = new double[size];
	}

	double halfSize = (double)size / (double)2;
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			realDistance[x][y] = sqrt( ( ( x - halfSize ) / halfSize ) * ( ( x - halfSize ) / halfSize ) + ( ( y - halfSize ) / halfSize ) * ( ( y - halfSize ) / halfSize ) );
		}
	}
	return realDistance;
}


double* RadialFrequency::SortDifferentDistances( double **realDistance, int size )
{
	double *differentValues = new double[size * size];
	int difPointer = 0;
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			for (int i = 0; i < difPointer + 1; i++)
			{
				if ( i == difPointer ) // if new distance isnot already in array;
				{
					differentValues[difPointer] = realDistance[x][y];
					difPointer++;
					break;
				}
				if ( differentValues[i] == realDistance[x][y] ) break;
			}
		}
	}
	/* Sort different Values */
	for (int i = 0; i < difPointer; i++)
	{
		for (int j = 1; j < difPointer; j++)
		{
			if ( differentValues[j] < differentValues[j - 1] ) 
			{
				double temp = differentValues[j];
				differentValues[j] = differentValues[j - 1];
				differentValues[j - 1] = temp;
			}
		}
	}
	double *result = new double[difPointer];
	for (int i = 0; i < difPointer; i++)
	{
		result[i] = differentValues[i];
	}
	numOfDifferentValues = difPointer;
	delete[] differentValues;
	return result;
}


void RadialFrequency::SetIntDistances( __in double **realDistance, __in double *differentDistances, __in int size )
{
	distance = new int*[size];
	for (int i = 0; i < size; i++) distance[i] = new int[size];

	double highestValue = differentDistances[numOfDifferentValues - 1];

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			for (int i = 0; i < numOfDifferentValues; i++)
			{
				if ( differentDistances[i] == realDistance[x][y] )
				{
					distance[x][y] = realDistance[x][y] / highestValue * resolution;
					break;
				}
			}
		}
	}
}


void RadialFrequency::CountOccurrences(int size)
{
	numeberOfOccurrences = new int[resolution];
	for (int i = 0; i < resolution; i++)
	{
		numeberOfOccurrences[i] = 0;
	}

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			numeberOfOccurrences[distance[x][y]]++;
		}
	}
}