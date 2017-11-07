#pragma once
#include "MagnitudeSpectrum.h"
#include <cmath>

using namespace std;

struct MMMData
{
	double max[4], min[4], sum[4];
	/*             
	                                    3/4               
	                  1/2               #############    
    1/4               #############     #           #     
	#############     #           #     #           #    
	#           #	  #           #	    #           # 	 
	############# 	  #############	    #############	 
	max, min, mean	  max, min, mean	max, min, mean	 
	*/
};

class LargeBlock
{
public:
	inline LargeBlock()
	{
		finalValue = 0;
		baseIndex = 0;

		for (int i = 0; i < 4; i++)
		{
			mmmDataDown.max[i] = -1;
			mmmDataDown.min[i] = 1000;
			mmmDataDown.sum[i] = 0;

			mmmDataRight.max[i] = -1;
			mmmDataRight.min[i] = 1000;
			mmmDataRight.sum[i] = 0;
		}
	}


	//quaters info
	MMMData mmmDataDown;
	MMMData mmmDataRight;

	//base index of first pixel in block
	int baseIndex;

	//final value
	BYTE finalValue;


	void CountFirst( Image *gray, MagnitudeSpectrum *pMagnitude, double** ppPixelValue ); // only first block
	void CountFirstInColumn( double **pixelValue, Image *gray, MagnitudeSpectrum *pMagnitude ); 
	void CountFirstInRow(  double **pixelValue, LargeBlock *above, Image *gray, MagnitudeSpectrum *pMagnitude );
	void Count( double** pixelValue, LargeBlock *above, LargeBlock *left, Image *gray, MagnitudeSpectrum *pMagnitude );


	
	inline double  PixelLuminance( int pixelValue )
	{
		return ( 0.7656 + 0.0364 * pixelValue ) * ( 0.7656 + 0.0364 * pixelValue );   //fast, unaccurate
		//return pow( 0.7656 + 0.0364 * pixelValue, 2.2 ); //slow, accurate
	}
	inline bool ContrastIsZerov( UINT max, UINT min, int mean, UINT fourierReduction )
	{
		// Default fourierReducion == 2
		if ( ( max - min <= fourierReduction ) ) return true;
		return false;
	}

private:
	//--For first row
	void CountQuarterFromPixelValue( int quarterToCount, double **pixelValue );
	void CountMMMDataFromPixelValue( double ** pixelValue );

	//--For next rows
	void CountMMMDataFromAbove( LargeBlock *above );


};

