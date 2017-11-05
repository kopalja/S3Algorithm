#pragma once

#include "ImageHandler.h"
#include "LargeBlock.h"
#include "FFT2D.h"

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
	void CountFirstRow( LargeBlock* rowToCount, Image *gray, MagnitudeSpectrum *pMagnitude );
	void CountRow( double **pixelValue, LargeBlock* rowToCount, LargeBlock *rowAbove, Image *gray, int y, MagnitudeSpectrum *pMagnitude );
	inline void SetBuffer(LargeBlock* row1, LargeBlock* row2, LargeBlock* row3, LargeBlock* row4, Image *result, int y);
	inline void SetRowBuffer(LargeBlock **blocks, int blocksLength, Image *result, int y);
	inline void SetPixelBlock(int color, Image *result, int index);

	int numberOfBlocks;


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
	}
};

