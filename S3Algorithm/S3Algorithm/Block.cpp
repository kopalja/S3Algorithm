#include "stdafx.h"
#include "Block.h"
#include <algorithm>
#include <iostream>

using namespace std;



int Block::shift = sizeof(int) * CHAR_BIT - 1;

void Block::CountQuater(int quaterToCount, Image* image)
{
	int *quater;
	int baseX = x;
	int baseY = y;
	UINT offSet = baseY * image->width;
	center = CountSubBlock( offSet + image->width + image->width + image->width + baseX + 3, image );
	if (quaterToCount == 1) 
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
	quater[0] = CountSubBlock( offSet + baseX + 0, image );
	quater[1] = CountSubBlock( offSet + baseX + 1, image );
	quater[2] = CountSubBlock( offSet + baseX + 2, image );
	quater[3] = CountSubBlock( offSet + image->width + baseX + 0, image );
	quater[4] = CountSubBlock( offSet + image->width + baseX + 1, image );
	quater[5] = CountSubBlock( offSet + image->width + baseX + 2, image );
	quater[6] = CountSubBlock( offSet + image->width + image->width + baseX + 0, image );
	quater[7] = CountSubBlock( offSet + image->width + image->width + baseX + 1, image );
	quater[8] = CountSubBlock( offSet + image->width + image->width + baseX + 2, image );
}

void Block::CountLine(int lineToCount, Image* image)
{
	if ( lineToCount == 1 )
	{
		line1[0] = CountSubBlock( ( y + 0) * image->width + x + 3, image );
		line1[1] = CountSubBlock( ( y + 1) * image->width + x + 3, image );
		line1[2] = CountSubBlock( ( y + 2) * image->width + x + 3, image );
	}
	else if ( lineToCount == 2 )
	{
		line2[0] = CountSubBlock( ( y + 3) * image->width + x + 0, image );
		line2[1] = CountSubBlock( ( y + 3) * image->width + x + 1, image );
		line2[2] = CountSubBlock( ( y + 3) * image->width + x + 2, image );
	}
	else if ( lineToCount == 3 )
	{
		line3[0] = CountSubBlock( ( y + 3) * image->width + x + 4, image );
		line3[1] = CountSubBlock( ( y + 3) * image->width + x + 5, image );
		line3[2] = CountSubBlock( ( y + 3) * image->width + x + 6, image );
	}
	else if ( lineToCount == 4 )
	{
		line4[0] = CountSubBlock( ( y + 4) * image->width + x + 3, image );
		line4[1] = CountSubBlock( ( y + 5) * image->width + x + 3, image );
		line4[2] = CountSubBlock( ( y + 6) * image->width + x + 3, image );
	}
}



inline int Block::CountSubBlock(int index, Image* image)
{
	int indexPlusWidth = index + image->width;

	int in1 = image->buffer[index] - image->buffer[index + 1];
	int const mask1 = in1 >> shift;

	int in2 = image->buffer[index] - image->buffer[indexPlusWidth];
	int const mask2 = in2 >> shift;

	int in3 = image->buffer[index] - image->buffer[indexPlusWidth + 1];
	int const mask3 = in3 >> shift;

	int in4 = image->buffer[index + 1] - image->buffer[indexPlusWidth];
	int const mask4 = in4 >> shift;

	int in5 = image->buffer[index + 1] - image->buffer[indexPlusWidth + 1];
	int const mask5 = in5 >> shift;

	int in6 = image->buffer[indexPlusWidth] - image->buffer[indexPlusWidth + 1];
	int const mask6 = in6 >> shift;

	return ((in1 + mask1) ^ mask1) + ((in2 + mask2) ^ mask2) + ((in3 + mask3) ^ mask3) + ((in4 + mask4) ^ mask4) + ((in5 + mask5) ^ mask5) + ((in6 + mask6) ^ mask6);
}


int Block::GetMax()
{
	int ans = 0;
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

	ans *=3;

	  //if (ans > 100) cout << ans << endl;

	if ( ans > 255 ) {  return 255; }
	return ans;
}