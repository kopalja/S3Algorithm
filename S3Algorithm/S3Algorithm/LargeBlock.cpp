#include "stdafx.h"
#include "LargeBlock.h"
#include <iostream>

using namespace std;


//--------------------First Row--------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

void LargeBlock::CountFirst( Image *gray, MagnitudeSpectrum *pMagnitude, double** ppPixelValue )
{
	baseIndex = 0;
	
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 32; x++)
		{
			ppPixelValue[x][y] = PixelLuminance( gray->buffer[y * gray->width + x] );
		}
	}
	CountMMMDataFromPixelValue( ppPixelValue );

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024, pMagnitude->m_FourierReduction ) ) finalValue = 0;
	else finalValue = pMagnitude->GetSlope( baseIndex, gray );
}


void LargeBlock::CountFirstInColumn( double **pixelValue, Image *gray, MagnitudeSpectrum *pMagnitude )
{
	for (int y = 0; y < 32; y++)
	{
		//shift data left
		for (int x = 0; x < 24; x++)
		{
			pixelValue[x][y] = pixelValue[x + 8][y];
		}
		//count rest
		for (int x = 24; x < 32; x++)
		{
			pixelValue[x][y] = PixelLuminance( gray->buffer[y * gray->width + baseIndex + x] );
		}
	}
	CountMMMDataFromPixelValue( pixelValue );

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024, pMagnitude->m_FourierReduction ) ) finalValue = 0;
	else finalValue = pMagnitude->GetSlope( baseIndex, gray );
}


void LargeBlock::CountMMMDataFromPixelValue( double **pixelValue )
{
	CountQuarterFromPixelValue( 0, pixelValue );
	CountQuarterFromPixelValue( 1, pixelValue );
	CountQuarterFromPixelValue( 2, pixelValue );
	CountQuarterFromPixelValue( 3, pixelValue );


	//
	// infoForAbove[0] is already corect
	//

	mmmDataDown.max[1] = max( mmmDataDown.max[0], mmmDataDown.max[1] );  
	mmmDataDown.min[1] = min( mmmDataDown.min[0], mmmDataDown.min[1] );
	mmmDataDown.sum[1] =  mmmDataDown.sum[0] + mmmDataDown.sum[1];

	mmmDataDown.max[2] = max( mmmDataDown.max[1], mmmDataDown.max[2] );  
	mmmDataDown.min[2] = min( mmmDataDown.min[1], mmmDataDown.min[2] );
	mmmDataDown.sum[2] =  mmmDataDown.sum[1] + mmmDataDown.sum[2];

	mmmDataDown.max[3] = max( mmmDataDown.max[2], mmmDataDown.max[3] );  
	mmmDataDown.min[3] = min( mmmDataDown.min[2], mmmDataDown.min[3] );
	mmmDataDown.sum[3] =  mmmDataDown.sum[2] + mmmDataDown.sum[3];
}


void LargeBlock::CountQuarterFromPixelValue( int quarterToCount, double **pixelValue )
{
	int yStart = 24;
	if ( quarterToCount == 1 )
	{
		yStart = 16;
	}
	else if ( quarterToCount == 2 )
	{
		yStart = 8;
	}
	else if ( quarterToCount == 3 )
	{
		yStart = 0;
	}

	for (int y = yStart; y < yStart + 8; y++)
	{
		for (int x = 0; x < 32; x++)
		{
			if ( pixelValue[x][y] > mmmDataDown.max[quarterToCount] ) // check max
			{
				mmmDataDown.max[quarterToCount] = pixelValue[x][y];
			}
			if ( pixelValue[x][y] < mmmDataDown.min[quarterToCount] ) // check min
			{
				mmmDataDown.min[quarterToCount] = pixelValue[x][y];
			}
			mmmDataDown.sum[quarterToCount] += pixelValue[x][y];
		}
	}
}


//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------



void LargeBlock::CountFirstInRow( double **pixelValue, LargeBlock *above, Image *gray, MagnitudeSpectrum *pMagnitude )
{

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 32; x++)
		{
			pixelValue[x][y] = PixelLuminance( gray->buffer[baseIndex + x + ( ( 24 + y ) * gray->width )] );
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int y = 0; y < 8; y++)
		{
			for (int x = ( 3 - i ) * 8; x < ( 4 - i ) * 8; x++)
			{
				if ( pixelValue[x][y] > mmmDataRight.max[i] )  mmmDataRight.max[i] = pixelValue[x][y];
				if ( pixelValue[x][y] < mmmDataRight.min[i] ) mmmDataRight.min[i] = pixelValue[x][y];
				mmmDataRight.sum[i] += pixelValue[x][y];
			}
		}
	}

	mmmDataRight.max[1] = max( mmmDataRight.max[0], mmmDataRight.max[1] );  
	mmmDataRight.min[1] = min( mmmDataRight.min[0], mmmDataRight.min[1] );
	mmmDataRight.sum[1] =  mmmDataRight.sum[0] + mmmDataRight.sum[1];

	mmmDataRight.max[2] = max( mmmDataRight.max[1], mmmDataRight.max[2] );  
	mmmDataRight.min[2] = min( mmmDataRight.min[1], mmmDataRight.min[2] );
	mmmDataRight.sum[2] =  mmmDataRight.sum[1] + mmmDataRight.sum[2];

	mmmDataRight.max[3] = max( mmmDataRight.max[2], mmmDataRight.max[3] );  
	mmmDataRight.min[3] = min( mmmDataRight.min[2], mmmDataRight.min[3] );
	mmmDataRight.sum[3] =  mmmDataRight.sum[2] + mmmDataRight.sum[3];

	CountMMMDataFromAbove( above );

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024, pMagnitude->m_FourierReduction ) ) finalValue = 0;
	else finalValue = pMagnitude->GetSlope( baseIndex, gray );
}



void LargeBlock::Count( double** pixelValue, LargeBlock *above, LargeBlock *left, Image *gray, MagnitudeSpectrum *pMagnitude )
{
	int baseIndexOfCounting = baseIndex + ( 24 * gray->width ) + 24;

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			pixelValue[x][y] = PixelLuminance( gray->buffer[baseIndexOfCounting + ( y * gray->width ) + x]);
		}
	}



	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if ( pixelValue[x][y] > mmmDataRight.max[0] ) mmmDataRight.max[0] = pixelValue[x][y];
			if ( pixelValue[x][y] < mmmDataRight.min[0] ) mmmDataRight.min[0] = pixelValue[x][y];
			mmmDataRight.sum[0] += pixelValue[x][y];
		}
	}

	mmmDataRight.max[1] = max( mmmDataRight.max[0], left->mmmDataRight.max[0] );
	mmmDataRight.min[1] = min( mmmDataRight.min[0], left->mmmDataRight.min[0] );
	mmmDataRight.sum[1] = mmmDataRight.sum[0] + left->mmmDataRight.sum[0];

	mmmDataRight.max[2] = max( mmmDataRight.max[0], left->mmmDataRight.max[1] );
	mmmDataRight.min[2] = min( mmmDataRight.min[0], left->mmmDataRight.min[1] );
	mmmDataRight.sum[2] = mmmDataRight.sum[0] + left->mmmDataRight.sum[1];

	mmmDataRight.max[3] = max( mmmDataRight.max[0], left->mmmDataRight.max[2] );
	mmmDataRight.min[3] = min( mmmDataRight.min[0], left->mmmDataRight.min[2] );
	mmmDataRight.sum[3] = mmmDataRight.sum[0] + left->mmmDataRight.sum[2];

	CountMMMDataFromAbove( above );

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024, pMagnitude->m_FourierReduction ) ) finalValue = 0;
	else finalValue = pMagnitude->GetSlope( baseIndex, gray );
}




void LargeBlock::CountMMMDataFromAbove( LargeBlock *above )
{
	mmmDataDown.max[0] = mmmDataRight.max[3];
	mmmDataDown.min[0] = mmmDataRight.min[3];
	mmmDataDown.sum[0] = mmmDataRight.sum[3];

	mmmDataDown.max[1] = max( mmmDataDown.max[0], above->mmmDataDown.max[0] );
	mmmDataDown.min[1] = min( mmmDataDown.min[0], above->mmmDataDown.min[0] );
	mmmDataDown.sum[1] = mmmDataDown.sum[0] + above->mmmDataDown.sum[0];

	mmmDataDown.max[2] = max( mmmDataDown.max[0], above->mmmDataDown.max[1] );
	mmmDataDown.min[2] = min( mmmDataDown.min[0], above->mmmDataDown.min[1] );
	mmmDataDown.sum[2] = mmmDataDown.sum[0] + above->mmmDataDown.sum[1];

	mmmDataDown.max[3] = max( mmmDataDown.max[0], above->mmmDataDown.max[2] );
	mmmDataDown.min[3] = min( mmmDataDown.min[0], above->mmmDataDown.min[2] );
	mmmDataDown.sum[3] = mmmDataDown.sum[0] + above->mmmDataDown.sum[2];
}
