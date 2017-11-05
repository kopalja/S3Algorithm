#include "stdafx.h"
#include "S1.h"

void S1::CreateS1( __in int fourierReduciton, __in bool skin, __in Image* pGrayImage, __out Image* pS1Image )
{
	_ASSERT( pGrayImage->width % 8 == 0 && pGrayImage->height % 8 == 0 );

	MagnitudeSpectrum *pMagnitude = new MagnitudeSpectrum( 32, fourierReduciton, skin );


	numberOfBlocks = pGrayImage->width / 8 - 3;
	int numberOfRows = pGrayImage->height / 8  - 3;

	LargeBlock *row1 = new LargeBlock[numberOfBlocks];
	LargeBlock *row2 = new LargeBlock[numberOfBlocks];
	LargeBlock *row3 = new LargeBlock[numberOfBlocks];
	LargeBlock *row4 = new LargeBlock[numberOfBlocks];
	double **pixelValue = new double*[8];
	for (int i = 0; i < 8; i++)
	{
		pixelValue[i] = new double[8];
	}

	CountFirstRow( row1, pGrayImage, pMagnitude );
	//CR(row1, gray, 0 );
	SetBuffer( row1, row4, row3, row2, pS1Image, 0 );

	for (int i = 1; i < numberOfRows; i++)
	{
		if ( ( i & 3 ) == 0)
		{
			CountRow( pixelValue, row1, row4, pGrayImage, i, pMagnitude );
			//CR(row1, gray, i );
			SetBuffer( row1, row4, row3, row2, pS1Image, i );
		}
		else if ( ( i & 3 ) == 1 )
		{
			CountRow( pixelValue, row2, row1, pGrayImage, i, pMagnitude ); 
			//CR(row2, gray, i );
			SetBuffer( row2, row1, row4, row3, pS1Image, i );
		}			
		else if ( ( i & 3 ) == 2 )
		{
			CountRow( pixelValue, row3, row2, pGrayImage, i, pMagnitude );
			//CR(row3, gray, i );
			SetBuffer( row3, row2, row1, row4, pS1Image, i );
		}
		else if ( ( i & 3 ) == 3 )
		{
			CountRow( pixelValue, row4, row3, pGrayImage, i, pMagnitude );
			//CR(row4, gray, i );
			SetBuffer( row4, row3, row2, row1, pS1Image, i );
		}
	}

	delete[] row1;
	delete[] row2;
	delete[] row3;
	delete[] row4;
	for (int i = 0; i < 8; i++)
	{
		delete[] pixelValue[i];
	}
	delete[] pixelValue;
}


void S1::CountFirstRow( LargeBlock* rowToCount, Image *gray, MagnitudeSpectrum *pMagnitude )
{
	double **piexelValue = rowToCount[0].CountFirst( gray, pMagnitude );

	for (int i = 1; i < numberOfBlocks; i++)
	{
		rowToCount[i].baseIndex = i * 8;
		rowToCount[i].CountFirstInColumn( piexelValue, gray, pMagnitude );
	}

	for (int i = 0; i < 32; i++)
	{
		delete[] piexelValue[i];
	}
	delete[] piexelValue;
}


void S1::CountRow( double **pixelValue, LargeBlock* rowToCount, LargeBlock *rowAbove, Image *gray, int y, MagnitudeSpectrum *pMagnitude )
{
	for (int i = 0; i < numberOfBlocks; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			rowToCount[i].mmmDataDown.max[j] = -1;
			rowToCount[i].mmmDataDown.min[j] = 1000;
			rowToCount[i].mmmDataDown.sum[j] = 0;

			rowToCount[i].mmmDataRight.max[j] = -1;
			rowToCount[i].mmmDataRight.min[j] = 1000;
			rowToCount[i].mmmDataRight.sum[j] = 0;
		}
		rowToCount[i].finalValue = 0;
	}

	rowToCount[0].baseIndex = y * 8 * gray->width;
	rowToCount[0].CountFirstInRow( &rowAbove[0], gray, pMagnitude );

	for (int i = 1; i < numberOfBlocks; i++)
	{
		rowToCount[i].baseIndex = y * 8 * gray->width + i * 8;
		rowToCount[i].Count( pixelValue, &rowAbove[i], &rowToCount[i - 1], gray, pMagnitude );
	}
}

void S1::SetBuffer(LargeBlock* row1, LargeBlock* row2, LargeBlock* row3, LargeBlock* row4, Image *result, int y)
{
	if ( y == 0 )
	{
		LargeBlock *temp[1];
		temp[0] = row1;
		SetRowBuffer( temp, 1, result, y );
	}
	else if ( y == 1 )
	{
		LargeBlock *temp[2];
		temp[0] = row1;
		temp[1] = row2;
		SetRowBuffer( temp, 2, result, y );
	}
	else if ( y == 2 )
	{
		LargeBlock *temp[3];
		temp[0] = row1;
		temp[1] = row2;
		temp[2] = row3;
		SetRowBuffer( temp, 3, result, y );
	}
	else if ( y < result->height / 2 - 4 )
	{
		LargeBlock *temp[4];
		temp[0] = row1;
		temp[1] = row2;
		temp[2] = row3;
		temp[3] = row4;
		SetRowBuffer( temp, 4, result, y );
	}
	else
	{
		LargeBlock *temp[4];
		temp[0] = row1;
		temp[1] = row2;
		temp[2] = row3;
		temp[3] = row4;
		SetRowBuffer( temp, 4, result, y );

		temp[0] = row1;
		temp[1] = row2;
		temp[2] = row3;
		SetRowBuffer( temp, 3, result, y + 1 );

		temp[0] = row1;
		temp[1] = row2;
		SetRowBuffer( temp, 2, result, y + 2 );

		temp[0] = row1;
		SetRowBuffer( temp, 1, result, y + 3 );


	}
}

void S1::SetRowBuffer(LargeBlock **blocks, int blocksLength, Image *result, int y)
{
	int color = 0;
	for (int i = 0; i < blocksLength; i++)
	{
		color += blocks[i][0].finalValue;
	}
	SetPixelBlock(  color / blocksLength, result, y * 2 * result->width ); // first column
	color = 0;
	for (int i = 0; i < blocksLength; i++)
	{
		color += blocks[i][0].finalValue;
		color += blocks[i][1].finalValue;
	}
	SetPixelBlock(  color / ( blocksLength * 2 ), result, y * 2 * result->width + 2 ); // second column
	color = 0;
	for (int i = 0; i < blocksLength; i++)
	{
		color += blocks[i][0].finalValue;
		color += blocks[i][1].finalValue;
		color += blocks[i][2].finalValue;
	}
	SetPixelBlock(  color / ( blocksLength * 3 ), result, y * 2 * result->width + 4 ); // third column
	for (int i = 3; i < numberOfBlocks; i++)
	{
		color = 0;
		for (int j = 0; j < blocksLength; j++)
		{
			color += blocks[j][i - 0].finalValue;
			color += blocks[j][i - 1].finalValue;
			color += blocks[j][i - 2].finalValue;
			color += blocks[j][i - 3].finalValue;
		}
		SetPixelBlock(  color / ( blocksLength * 4 ), result, y * 2 * result->width + i * 2 );
	}
	color = 0;
	for (int i = 0; i < blocksLength; i++)
	{
		color += blocks[i][numberOfBlocks - 1].finalValue;
		color += blocks[i][numberOfBlocks - 2].finalValue;
		color += blocks[i][numberOfBlocks - 3].finalValue;
	}
	SetPixelBlock(  color / ( blocksLength * 3 ), result, y * 2 * result->width + numberOfBlocks * 2 );
	color = 0;
	for (int i = 0; i < blocksLength; i++)
	{
		color += blocks[i][numberOfBlocks - 1].finalValue;
		color += blocks[i][numberOfBlocks - 2].finalValue;
	}
	SetPixelBlock(  color / ( blocksLength * 2 ), result, y * 2 * result->width + ( numberOfBlocks + 1 ) * 2 );
	color = 0;
	for (int i = 0; i < blocksLength; i++)
	{
		color += blocks[i][numberOfBlocks - 1].finalValue;
	}
	SetPixelBlock(  color / blocksLength, result, y * 2 * result->width + ( numberOfBlocks + 2 ) * 2 );

}

void S1::SetPixelBlock(int color, Image *result, int index) // 8x8 of origin resolution
{
	result->buffer[index] = color;
	result->buffer[index + 1] = color;
	result->buffer[index + result->width] = color;
	result->buffer[index + result->width + 1] = color;
}
