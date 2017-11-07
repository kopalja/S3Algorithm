#pragma once
#include <Windows.h>


class RadialFrequency
{
public:
	RadialFrequency( __in int blockSize, __in int resolution );
	~RadialFrequency( void );

	double *m_pResultFunction;
	BYTE **m_ppDistance;

private:
	UINT m_BlockSize;

	void CountRealDistances( 
		__out double **ppRealDistance 
		);

	int FindDifferentDistances( 
		__in double **ppRealDistance, 
		__out_ecount( m_BlockSize * m_BlockSize ) double *pDifferentDistance
		);

	void SortDifferentDistances( 
		__in UINT numberOfDifferentDistances,
		__in_ecount( m_BlockSize * m_BlockSize ) double *pDifferentDistances, 
		__out_ecount( numberOfDifferentDistances )  double *pSortedDifferentDistances 
		);

	void SetIntDistances( 
		__in UINT numberOfDifferentDistances,
		__in UINT resolution,
		__in double **ppRealDistance, 
		__in_ecount( numberOfDifferentDistances ) double *pSortedDifferentDistances 
		);

	void ShiftDistances(
		);
};

