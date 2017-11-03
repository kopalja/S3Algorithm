// SharpnessMap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "S3.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	S3 s;
	UINT fourierReduction = 2;
	bool skin = true;

	UINT resultType = 3;
	if ( argc > 3 )
	{
		
		if ( argv[3][0] == 's' && argv[3][1] == '1' ) resultType = 1;
		if ( argv[3][0] == 's' && argv[3][1] == '2' ) resultType = 2;
		if ( argv[3][0] == 's' && argv[3][1] == '3' ) resultType = 3;

		if ( argc > 4 )
		{
			fourierReduction = atoi( argv[4] );
			if ( argc > 5 )
			{
				if ( argv[5][0] == 'g' )
					skin = false;
			}
		}

	}
	s.Process( argv[1], argv[2], resultType, fourierReduction, skin );
	return 0;
}

