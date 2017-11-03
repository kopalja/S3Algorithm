#pragma once
#include "FFT2D.h"
#include <iostream>
using namespace std;



class RadialFrequency
{
public:
	RadialFrequency( __in int blockSize, __in int resolution );
	~RadialFrequency( void );
	double *resultFunc;
	int **distance;
	int *numeberOfOccurrences;
	int resolution;


private:
	double** CountRealDistances( __in int size );
	double* SortDifferentDistances( double **realDistance, int size );
	void SetIntDistances( __in double **realDistance, __in double *differentDistances, __in int size );
	void CountOccurrences( int size );

	int numOfDifferentValues;
};

