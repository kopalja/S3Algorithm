#pragma once
#include "Image.h"

class Block
{
public:
	inline Block(void){};

	UINT quarter1[9];
	UINT quarter2[9];
	UINT quarter3[9];
	UINT quarter4[9];

	UINT line1[3];
	UINT line2[3];
	UINT line3[3];
	UINT line4[3];

	int center;

	// Coordinats of base pixel of block
	int x,y;

	BYTE finalValue;

	
	void CountQuater( 
		__in int quaterToCount, 
		__in Image *pImage
		);

	void CountLine( 
		__in int lineToCount,
		__in Image* pImage
		);

	int CountSubBlock( 
		__in int index, 
		__in Image* pImage
		);


	BYTE GetMax();
};

