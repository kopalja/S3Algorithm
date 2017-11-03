#include "stdafx.h"
#include "LargeBlock.h"
#include <iostream>

using namespace std;

RadialFrequency LargeBlock::radial( 32, 16 );
FFT2D LargeBlock::fourier( 32 );
LinearRegression LargeBlock::regression;
UINT LargeBlock::fourierReduction;
bool LargeBlock::skin;
//--------------------First Row--------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

double** LargeBlock::CountFirst( Image *gray )
{
	baseIndex = 0;
	double **pixelValue;
	pixelValue = new double*[32];
	for (int i = 0; i < 32; i++)
	{
		pixelValue[i] = new double[32];
	}
	
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 32; x++)
		{
			pixelValue[x][y] = PixelLuminance( gray->buffer[y * gray->width + x] );
		}
	}
	CountMMMDataFromPixelValue( pixelValue );

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024 ) ) finalValue = 0;
	else finalValue = Magic( gray );

	return pixelValue;
}


void LargeBlock::CountFirstInColumn( double **pixelValue, Image *gray )
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

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024 ) ) finalValue = 0;
	else finalValue = Magic( gray );
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


void LargeBlock::CountQuarterFromPixelValue(int quarterToCount, double **pixelValue)
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



void LargeBlock::CountFirstInRow( LargeBlock *above, Image *gray )
{
	double **subBlock0 = new double*[32];
	for (int i = 0; i < 32; i++)
	{
		subBlock0[i] = new double[8];
	}

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 32; x++)
		{
			subBlock0[x][y] = PixelLuminance( gray->buffer[baseIndex + x + ( ( 24 + y ) * gray->width )] );
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int y = 0; y < 8; y++)
		{
			for (int x = ( 3 - i ) * 8; x < ( 4 - i ) * 8; x++)
			{
				if ( subBlock0[x][y] > mmmDataRight.max[i] )  mmmDataRight.max[i] = subBlock0[x][y];
				if ( subBlock0[x][y] < mmmDataRight.min[i] ) mmmDataRight.min[i] = subBlock0[x][y];
				mmmDataRight.sum[i] += subBlock0[x][y];
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

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024 ) ) finalValue = 0;
	else finalValue = Magic( gray );

	for (int i = 0; i < 32; i++)
	{
		delete[] subBlock0[i];
	}
	delete[] subBlock0;
}



void LargeBlock::Count( double** pixelValue, LargeBlock *above, LargeBlock *left, Image *gray )
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

	if ( ContrastIsZerov( mmmDataDown.max[3], mmmDataDown.min[3], mmmDataDown.sum[3] / 1024 ) ) finalValue = 0;
	else finalValue = Magic( gray );
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



int LargeBlock::Magic( Image *grayImage )
{
	int size = 32;
	int vectorLength = 4;
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			fourier.complex[x][y].Real = grayImage->buffer[ baseIndex + y * grayImage->width + x ];
			fourier.complex[x][y].Imaginary = 0;
		}
	}
	fourier.Process( fourier.complex );
	for (int i = 0; i < vectorLength; i++)
	{
		radial.resultFunc[i] = 0;
	}
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if ( radial.distance[j][i] < vectorLength )
			radial.resultFunc[ radial.distance[j][i] ] += sqrt(  fourier.complex[j][i].Real * fourier.complex[j][i].Real + fourier.complex[j][i].Imaginary * fourier.complex[j][i].Imaginary );
		}
	}
	for (int i = 0; i < vectorLength; i++)
	{
		radial.resultFunc[i] = log( radial.resultFunc[i] ) / fourier.log2;
	}


	
	double alfa = -1 * regression.GetAlpha( radial.resultFunc, vectorLength  );


	if ( skin )
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