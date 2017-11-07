#include "stdafx.h"
#include "Block.h"
#include <algorithm>
#include <iostream>

using namespace std;




void Block::CountQuater( __in int quaterToCount, __in Image* pImage )
{
	UINT *quater;
	int baseX = x;
	int baseY = y;
	UINT offSet = baseY * pImage->width;
	center = CountSubBlock( offSet + pImage->width + pImage->width + pImage->width + baseX + 3, pImage );
	if ( quaterToCount == 1 ) 
	{
		quater = quarter1;
	}
	else if (quaterToCount == 2) 
	{
		quater = quarter2;
		baseX += 4;
	}
	else if (quaterToCount == 3)
	{
		quater = quarter3;
		baseY += 4;
	}
	else if (quaterToCount == 4)
	{
		quater = quarter4;
		baseX += 4;
		baseY += 4;
	}
	quater[0] = CountSubBlock( offSet + baseX + 0, pImage );
	quater[1] = CountSubBlock( offSet + baseX + 1, pImage );
	quater[2] = CountSubBlock( offSet + baseX + 2, pImage );
	quater[3] = CountSubBlock( offSet + pImage->width + baseX + 0, pImage );
	quater[4] = CountSubBlock( offSet + pImage->width + baseX + 1, pImage );
	quater[5] = CountSubBlock( offSet + pImage->width + baseX + 2, pImage );
	quater[6] = CountSubBlock( offSet + pImage->width + pImage->width + baseX + 0, pImage );
	quater[7] = CountSubBlock( offSet + pImage->width + pImage->width + baseX + 1, pImage );
	quater[8] = CountSubBlock( offSet + pImage->width + pImage->width + baseX + 2, pImage );
}

void Block::CountLine( __in int lineToCount, __in Image* pImage )
{
	if ( lineToCount == 1 )
	{
		line1[0] = CountSubBlock( ( y + 0) * pImage->width + x + 3, pImage );
		line1[1] = CountSubBlock( ( y + 1) * pImage->width + x + 3, pImage );
		line1[2] = CountSubBlock( ( y + 2) * pImage->width + x + 3, pImage );
	}
	else if ( lineToCount == 2 )
	{
		line2[0] = CountSubBlock( ( y + 3) * pImage->width + x + 0, pImage );
		line2[1] = CountSubBlock( ( y + 3) * pImage->width + x + 1, pImage );
		line2[2] = CountSubBlock( ( y + 3) * pImage->width + x + 2, pImage );
	}
	else if ( lineToCount == 3 )
	{
		line3[0] = CountSubBlock( ( y + 3) * pImage->width + x + 4, pImage );
		line3[1] = CountSubBlock( ( y + 3) * pImage->width + x + 5, pImage );
		line3[2] = CountSubBlock( ( y + 3) * pImage->width + x + 6, pImage );
	}
	else if ( lineToCount == 4 )
	{
		line4[0] = CountSubBlock( ( y + 4) * pImage->width + x + 3, pImage );
		line4[1] = CountSubBlock( ( y + 5) * pImage->width + x + 3, pImage );
		line4[2] = CountSubBlock( ( y + 6) * pImage->width + x + 3, pImage );
	}
}



inline int Block::CountSubBlock( __in int index, __in Image* pImage)
{
	int indexPlusWidth = index + pImage->width;
	int shift = sizeof(int) * CHAR_BIT - 1;

	int in1 = pImage->buffer[index] - pImage->buffer[index + 1];
	int const mask1 = in1 >> shift;

	int in2 = pImage->buffer[index] - pImage->buffer[indexPlusWidth];
	int const mask2 = in2 >> shift;

	int in3 = pImage->buffer[index] - pImage->buffer[indexPlusWidth + 1];
	int const mask3 = in3 >> shift;

	int in4 = pImage->buffer[index + 1] - pImage->buffer[indexPlusWidth];
	int const mask4 = in4 >> shift;

	int in5 = pImage->buffer[index + 1] - pImage->buffer[indexPlusWidth + 1];
	int const mask5 = in5 >> shift;

	int in6 = pImage->buffer[indexPlusWidth] - pImage->buffer[indexPlusWidth + 1];
	int const mask6 = in6 >> shift;

	return ((in1 + mask1) ^ mask1) + ((in2 + mask2) ^ mask2) + ((in3 + mask3) ^ mask3) + ((in4 + mask4) ^ mask4) + ((in5 + mask5) ^ mask5) + ((in6 + mask6) ^ mask6);
}


BYTE Block::GetMax()
{
	UINT ans = 0;
	for (int i = 0; i < 9; i++)
	{
		if ( quarter1[i] > ans ) ans = quarter1[i];
		if ( quarter2[i] > ans ) ans = quarter2[i];
		if ( quarter3[i] > ans ) ans = quarter3[i];
		if ( quarter4[i] > ans ) ans = quarter4[i];
	}
	for (int i = 0; i < 3; i++)
	{
		if ( line1[i] > ans ) ans = line1[i];
		if ( line2[i] > ans ) ans = line2[i];
		if ( line3[i] > ans ) ans = line3[i];
		if ( line4[i] > ans ) ans = line4[i];
	}
	if (center > ans ) ans = center;

	ans = ans << 1;

	if ( ans > 255 ) 
		return 255;

	return ans;
}