#pragma once
#include "Image.h"

class Block
{
public:
	inline Block(void){};

	int quarter1[9];
	int quarter2[9];
	int quarter3[9];
	int quarter4[9];

	int line1[3];
	int line2[3];
	int line3[3];
	int line4[3];

	int center;

	// Coordinats of base pixel of block
	int x,y;

	int finalValue;

	static int shift;

	
	void CountQuater(int quaterToCount, Image* image);
	void CountLine(int lineToCount, Image* image);

	int CountSubBlock(int index, Image* image);


	int GetMax();
};

