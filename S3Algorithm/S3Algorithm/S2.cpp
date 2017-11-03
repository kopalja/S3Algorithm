#include "stdafx.h"
#include "S2.h"


void S2::CreateS2( __in Image* pGrayImage, __out Image* pS2Image )
{
	_ASSERT( pGrayImage->width % 4 == 0 && pGrayImage->height % 4 == 0 );


	numberOfBlocks = pS2Image->width - 1;
	Block *rowA = new Block[numberOfBlocks];
	Block *rowB = new Block[numberOfBlocks];


	CountFirstRow( rowA, pGrayImage );

	SetBuffer( rowA, nullptr, pS2Image->buffer, 0, pS2Image->height ); 
	int temp = pS2Image->height - 1;
	for (int i = 1; i < temp; i++)
	{
		if ( i % 2 == 0 )
		{
			CountRow( rowA, rowB, pGrayImage, i );
			SetBuffer( rowB, rowA, pS2Image->buffer, i, temp - 1 ); 
		}
		else 
		{
			CountRow( rowB, rowA, pGrayImage, i );
			SetBuffer( rowA, rowB, pS2Image->buffer, i, temp - 1 );
		}
	}
	delete[] rowA;
	delete[] rowB;
}

void S2::CountFirstRow(Block* rowToCount, Image* image)
{
	for (int i = 0; i < numberOfBlocks; i++)
	{
		rowToCount[i].x = i * 4;
		rowToCount[i].y = 0;

		rowToCount[i].CountQuater( 1, image );
		rowToCount[i].CountQuater( 2, image );
		rowToCount[i].CountQuater( 3, image );
		rowToCount[i].CountQuater( 4, image );

		rowToCount[i].CountLine( 1, image );
		rowToCount[i].CountLine( 2, image );
		rowToCount[i].CountLine( 3, image );
		rowToCount[i].CountLine( 4, image );

		rowToCount[i].finalValue = rowToCount[i].GetMax();
	}
}


void S2::CountRow(Block* rowToCount, Block* rowAbove, Image* image, int y)
{
	rowToCount[0].x = 0;
	rowToCount[0].y = y << 2;
	for (int i = 0; i < 9; i++)
	{
		rowToCount[0].quarter1[i] = rowAbove[0].quarter3[i];
		rowToCount[0].quarter2[i] = rowAbove[0].quarter4[i];
	}
	for (int i = 0; i < 3; i++)
	{
		rowToCount[0].line1[i] = rowAbove[0].line4[i];
	}
	rowToCount[0].CountQuater( 3, image );
	rowToCount[0].CountQuater( 4, image );
	rowToCount[0].CountLine( 2, image );
	rowToCount[0].CountLine( 3, image );
	rowToCount[0].CountLine( 4, image );
	rowToCount[0].finalValue = rowToCount[0].GetMax();


	for (int i = 1; i < numberOfBlocks; i++)
	{
		rowToCount[i].x = i << 2;
		rowToCount[i].y = y << 2;
		for (int j = 0; j < 9; j++)
		{
			rowToCount[i].quarter1[j] = rowAbove[i].quarter3[j];
			rowToCount[i].quarter2[j] = rowAbove[i].quarter4[j];
			rowToCount[i].quarter3[j] = rowToCount[i - 1].quarter4[j];
		}
		for (int j = 0; j < 3; j++)
		{
			rowToCount[i].line1[j] = rowAbove[i].line4[j];
			rowToCount[i].line2[j] = rowToCount[i - 1].line3[j];
		}
		rowToCount[i].CountQuater( 4, image );
		rowToCount[i].CountLine( 3, image );  // Count, slow....
		rowToCount[i].CountLine( 4, image );
		rowToCount[i].finalValue = rowToCount[i].GetMax();
	}
}


void S2::SetBuffer(Block* upRow, Block* downRow, BYTE* buffer, int y, int height)
{
	if ( y == 0 ) // first row
	{
		buffer[0] = upRow[0].finalValue;
		for (int i = 1; i < numberOfBlocks; i++)
		{
			buffer[i] = ( upRow[i].finalValue + upRow[i - 1].finalValue ) >> 1;
		}
		buffer[numberOfBlocks] = upRow[numberOfBlocks - 1].finalValue;
	}
	else
	{
		buffer[y * ( numberOfBlocks + 1 )] = ( upRow[0].finalValue + downRow[0].finalValue ) >> 1;
		for (int i = 1; i < numberOfBlocks; i++)
		{
			buffer[y * ( numberOfBlocks + 1 ) + i] = ( upRow[i - 1].finalValue + upRow[i].finalValue + downRow[i - 1].finalValue + downRow[i].finalValue ) >> 2;
		}
		buffer[y * ( numberOfBlocks + 1 ) + numberOfBlocks] = ( upRow[numberOfBlocks - 1].finalValue + downRow[numberOfBlocks - 1].finalValue ) >> 1;
	}
	if ( y == height ) // last row
	{
		buffer[( y + 1 ) * ( numberOfBlocks + 1 )] = downRow[0].finalValue;
		for (int i = 1; i < numberOfBlocks; i++)
		{
			buffer[( y + 1 ) * ( numberOfBlocks + 1 ) + i] = ( downRow[i - 1].finalValue + downRow[i].finalValue ) >> 1;
		}
		buffer[( y + 1 ) * ( numberOfBlocks + 1 ) + numberOfBlocks] = downRow[numberOfBlocks - 1].finalValue;
	}
}
