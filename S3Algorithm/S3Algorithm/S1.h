#pragma once

#include "LargeBlock.h"


class S1
{
public:

	void CreateS1( 
		__in int fourierReduciton,
		__in bool skin,
		__in Image* pGrayImage, 
		__out Image* pS1Image
		);

private:

	void CountFirstRow( 
		__out_ecount( numberOfBlocks ) LargeBlock* rowToCount,
		__in Image *gray, 
		__in MagnitudeSpectrum *pMagnitude 
		);

	void CountRow( 
		_Out_writes_( 256 ) double **pp256PixelMaP, 
		_Out_writes_( 64 ) double **pp64PixelMaP, 
		__out_ecount( numberOfBlocks ) LargeBlock* rowToCount, 
		__in_ecount( numberOfBlocks ) LargeBlock *rowAbove, 
		__in Image *gray, 
		__in int rowNumber,
		__in MagnitudeSpectrum *pMagnitude 
		);

	inline void SetBuffer(
		__in_ecount( numberOfBlocks ) LargeBlock* row1, 
		__in_ecount( numberOfBlocks ) LargeBlock* row2, 
		__in_ecount( numberOfBlocks ) LargeBlock* row3, 
		__in_ecount( numberOfBlocks ) LargeBlock* row4,
		__out Image *result, 
		__in int rowNumber
		);

	inline void SetRowBuffer(
		__in LargeBlock **blocks, 
		__in int blocksLength, 
		__out Image *result,
		__in int rowNumber
		);

	inline void SetPixelBlock(
		__in int color, 
		__out Image *result, 
		__in int index
		);

	int numberOfBlocks;




	/*
	inline void CR(LargeBlock *row, Image *gray, int yy)
	{
		double **pixelValue = new double*[32];
		for (int i = 0; i < 32; i++)
		{
			pixelValue[i] = new double[32];
		}

		double max = -1;
		double min = 1000;
		double sum = 0;
		for (int i = 0; i < numberOfBlocks; i++)
		{
			double max = -1;
			double min = 1000;
			double sum = 0;
			for (int y = 0; y < 32; y++)
			{
				for (int x = 0; x < 32; x++)
				{
					pixelValue[x][y] = row[0].PixelLuminance( gray->buffer[ (yy * 8 + y) * gray->width + i * 8 + x] );
					max = max(max, pixelValue[x][y]);
					min = min(min, pixelValue[x][y]);
					sum += pixelValue[x][y];
				}
			}

			if ( row[0].ContrastIsZerov( max, min, sum / 1024, 2 ) ) row[i].finalValue = 0;
			else row[i].finalValue = 255;
		}
	} */
};

