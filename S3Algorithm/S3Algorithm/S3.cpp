#include "stdafx.h"
#include "S3.h"
#include <iostream>
#include <ctime>
#include <math.h>
#include <crtdbg.h>

#include <cmath>
#include "LinearRegression.h"


using namespace std;




S3::S3( void )
{
	#ifdef NDEBUG
		cout << "This is release" << endl;
	#else
		cout << "This is debug" << endl;
	#endif
}

S3::~S3( void )
{
}

	
void S3::Process(  char* inputPath, char* outputPath, int resultType, int fourierReduction, bool skin )
{
	HRESULT hr = ReadParams( resultType );
	if ( SUCCEEDED( hr ) )
	{
		ImageHandler imageHandler( inputPath, outputPath, &hr );
		if ( SUCCEEDED( hr ) )
		{
			/* Aloc origin image */
			Image *pOrigin = new Image(
				imageHandler.InputImageWidth(), 
				imageHandler.InputImageHeight(), 
				imageHandler.InputImageWidth() * imageHandler.InputImageHeight() * imageHandler.InputImageByteDepth(),
				new BYTE[imageHandler.InputImageWidth() * imageHandler.InputImageHeight() * imageHandler.InputImageByteDepth()] 
			);
			hr = imageHandler.Create( pOrigin );
			if ( SUCCEEDED( hr ) )
			{
				/* Aloc GrayImage from origin */
				UINT grayWidth, grayHeight, graySize;
				GetGrayScaleSize( pOrigin->width, pOrigin->height, &grayWidth, &grayHeight, &graySize );
				Image *pGrayImage = new Image( grayWidth, grayHeight, graySize, new BYTE[graySize] );
				GrayScale( pOrigin, pGrayImage );
				delete pOrigin;

				/* Aloc ResultImage from grayImage */
				Image *pResultImage = new Image( grayWidth >> 2, grayHeight >> 2, graySize >> 4, new BYTE[graySize >> 4] );
				GrayToResult( fourierReduction, skin, pGrayImage, pResultImage );
				delete pGrayImage;

				/* Save ResultImage */ 
				hr = imageHandler.Save( pResultImage );
				delete pResultImage;
			}
		}
	}
}

HRESULT S3::ReadParams( __in int resultType )
{
	if ( resultType == 1 )
	{
		m_resultType = ResultType::S1Image;
	}
	else if ( resultType == 2 )
	{
		m_resultType = ResultType::S2Image;
	}
	else if ( resultType == 3 )
	{
		m_resultType = ResultType::S3Image;
	}
	else 
	{
		return E_INVALIDARG;
	}
	return S_OK;
}



void S3::GetGrayScaleSize( __in UINT imageWidth, __in UINT imageHeight, __out UINT *pWidth, __out UINT *pHeight, __out UINT *pSize )
{
	BYTE restX = 8 - ( imageWidth % 8 );
	BYTE restY = 8 - ( imageHeight % 8 );
	if ( restX == 8 ) restX = 0;
	if ( restY == 8 ) restY = 0;

	*pWidth = imageWidth + restX;
	*pHeight = imageHeight + restY;
	*pSize = *pWidth * *pHeight;
}



void S3::GrayScale( __in Image *pOrigin, __out Image *pGrayImage )
{
	UINT byteDepth = 0;
	if ( pOrigin->size == pOrigin->width * pOrigin->height * 3 )
		byteDepth = 3;
	else if ( pOrigin->size == pOrigin->width * pOrigin->height * 4 )
		byteDepth = 4;
	_ASSERT( byteDepth == 3 || byteDepth == 4 );


	int j = 0, i = 0;
	for (int y = 0; y < pOrigin->height; y++)
	{
		for (int x = 0; x < pOrigin->width; x++)
		{
			int gray = 2989 * pOrigin->buffer[i + 2] + 5870 * pOrigin->buffer[i + 1] + 1140 * pOrigin->buffer[i];
			pGrayImage->buffer[j++] = gray / 10000;
			i += byteDepth;
		}
		for (int x = 0; x < pGrayImage->width - pOrigin->width; x++)
		{
			pGrayImage->buffer[j++] = 127;
		}
	}
	for (int y = 0; y < pGrayImage->height - pOrigin->height; y++)
	{
		for (int x = 0; x < pGrayImage->width; x++)
		{
			pGrayImage->buffer[j++] = 127;
		}
	}
}

void S3::GrayToResult( __in int fourierReduction, __in bool skin, __in Image *pGrayImage, __out Image *pResultImage )
{
	std::clock_t clock;
	int start = std::clock();
	int duration;

	/* Create S1Image from GrayScale */
	if ( m_resultType == ResultType::S1Image )
	{
		S1 s1Algorithm;
		s1Algorithm.CreateS1( fourierReduction, skin,  pGrayImage, pResultImage );
		duration = ( std::clock() - start ); cout << "S1 : " << duration << endl; start = std::clock();
		_ASSERTE( _CrtCheckMemory() );
	}
	
	/* Create S2Image from GrayScale */
	else if ( m_resultType == ResultType::S2Image )
	{
		S2 s2Algorithm;
		s2Algorithm.CreateS2( pGrayImage, pResultImage );
		duration = ( std::clock() - start ); cout << "S2 : " << duration << endl; start = std::clock();
		_ASSERTE( _CrtCheckMemory() );
	}

	/* Create S1Image and S2Image and Merge them */
	else if ( m_resultType == ResultType::S3Image )
	{
		Image *pTempImage = new Image( pResultImage->width, pResultImage->height, pResultImage->size, new BYTE[pResultImage->size] );

		S1 s1Algorithm;
		s1Algorithm.CreateS1( fourierReduction, skin, pGrayImage, pResultImage );
		duration = ( std::clock() - start ); cout << "S1 : " << duration << endl; start = std::clock();

		S2 s2Algorithm;
		s2Algorithm.CreateS2( pGrayImage, pTempImage );
		duration = ( std::clock() - start ); cout << "S2 : " << duration << endl; start = std::clock();

		Merge( pTempImage, pResultImage );
		duration = ( std::clock() - start ); cout << "Merge : " << duration << endl; start = std::clock();

		delete pTempImage;
		_ASSERTE( _CrtCheckMemory() );
	}
}


void S3::Merge( __in Image *pTempImage, __inout Image *pResultImage )
{
	_ASSERT( pTempImage->size == pResultImage->size && pTempImage->width == pResultImage->width && pTempImage->height == pResultImage->height );

	for (int i = 0; i < pResultImage->size; i++)
	{
		pResultImage->buffer[i] = sqrt( pTempImage->buffer[i] * pResultImage->buffer[i] ); 
	}
}
