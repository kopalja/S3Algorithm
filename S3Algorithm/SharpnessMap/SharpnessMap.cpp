// SharpnessMap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dirent.h"

#include "S3.h"

#include <direct.h>
#include <iostream>
#include <vector>
#include <thread>

using namespace std;

bool IsJPG( string fileName )
{

	int k = fileName.length() - 1;
	bool isJPG = false;
	if ( fileName[k] == 'g' )
		if ( fileName[k - 1] == 'p' )
			if ( fileName[k - 2] == 'j')
				if ( fileName[k - 3] == '\.' )
					isJPG = true;
	if ( fileName[k] == 'G' )
		if ( fileName[k - 1] == 'P' )
			if ( fileName[k - 2] == 'J')
				if ( fileName[k - 3] == '\.' )
					isJPG = true;

	return isJPG;

}

bool IsTIF( string fileName )
{

	int k = fileName.length() - 1;
	bool isTIF = false;
	if ( fileName[k] == 'f' )
		if ( fileName[k - 1] == 'i' )
			if ( fileName[k - 2] == 't' )
				if ( fileName[k - 3] == '\.' )
					isTIF = true;
	if ( fileName[k] == 'F' )
		if ( fileName[k - 1] == 'I' )
			if ( fileName[k - 2] == 'T' )
				if ( fileName[k - 3] == '\.' )
					isTIF = true;

	return isTIF;

}

void CallSharpnessMap( vector<string> *fileNames, UINT mapRequested, UINT fourierReduction, bool skin )
{
	S3 s;

	for (int i = 0; i < fileNames->size(); i++)
	{
		string folderName = "SharpnessMap\\";
		string pathToInput = fileNames->at(i);
		string pathToOutput = fileNames->at(i);

		int j = pathToOutput.length();
		while ( pathToOutput[j] != '\\' )
		{
			j--;
		}
		pathToOutput.insert( j + 1, folderName );

		j = pathToOutput.length();
		while ( pathToOutput[j] != '\.' )
		{
			j--;
		}
		pathToOutput.insert( j, "SM" );

		j = pathToOutput.length() - 1;
		pathToOutput[j--] = 'g';
		pathToOutput[j--] = 'p';
		pathToOutput[j--] = 'j';


		//char *input = new char[pathToInput.length() + 1];
		char *input = (char*)pathToInput.c_str();
		//char *output = new char[pathToOutput.length() + 1];
		char *output = (char*)pathToOutput.c_str();

		s.Process( input, output, mapRequested, fourierReduction, skin );
 	}

}

int main(int argc, char* argv[])
{
	S3 s;
	string secondArgument;
	int i = 0;
	char* c = argv[1];
	int mapRequested = 3;
	UINT fourierReduction = 2;
	bool skin = true;

	while ( c[i] != '\0' )
	{
		secondArgument += c[i];	
		i++;
	}

	bool IsOnlyOnePictureRequested = false;
	string FolderName;
	string InputPath;
	if ( IsJPG( secondArgument ) || IsTIF( secondArgument ))
	{
		IsOnlyOnePictureRequested = true;
		InputPath = secondArgument;
	}
	else
	{
		FolderName = secondArgument;
	}

	if ( argc == 3 )
	{
		if ( argv[2][0] == 's' )
			skin = true;
		if ( argv[2][0] == 'n' )
			skin = false;
		if ( ( argv[2][0] == '1' ) || ( argv[2][0] == '2' ) || ( argv[2][0] == '3' ) )
			mapRequested = argv[2][0] - 48;
	}
	if ( argc == 4 )
	{
		if ( ( argv[2][0] == '1' ) || ( argv[2][0] == '2' ) || ( argv[2][0] == '3' ) )
			mapRequested = argv[2][0] - 48;

		int j = 0;
		c = argv[3];
		bool IsProperNumber = true;
		fourierReduction = 0;

		while ( c[j] != '\0' )
		{
			j++;
		}
		int bound = j;
		
		j = 0;
		while ( j != bound )
		{
			if ( ( c[j] < 48 ) || ( c[j] > 57 ) )
			{
				IsProperNumber = false;
			}
			else
			{
				fourierReduction += ( c[j] - 48 ) * (int)pow( 10, bound - j - 1 );
			}
			j++;
		}
		if ( !IsProperNumber )
		{
			fourierReduction = 2;
		}
	}
	if ( argc == 5 )
	{
		if ( ( argv[2][0] == '1' ) || ( argv[2][0] == '2' ) || ( argv[2][0] == '3' ) )
			mapRequested = argv[2][0] - 48;

		int j = 0;
		c = argv[3];
		bool IsProperNumber = true;
		fourierReduction = 0;

		while ( c[j] != '\0' )
		{
			j++;
		}
		int bound = j;
		
		j = 0;
		while ( j != bound )
		{
			if ( ( c[j] < 48 ) || ( c[j] > 57 ) )
			{
				IsProperNumber = false;
			}
			else
			{
				fourierReduction += ( c[j] - 48 ) * (int)pow( 10, bound - j - 1 );
			}
			j++;
		}
		if ( !IsProperNumber )
		{
			fourierReduction = 2;
		}

 		if ( argv[4][0] == 's' )
			skin = true;
		if ( argv[4][0] == 'n' )
			skin = false;
	}

	if ( IsOnlyOnePictureRequested )
	{
		string OutputPath = InputPath;

		int j = OutputPath.length() - 1;
		OutputPath[j--] = 'g';
		OutputPath[j--] = 'p';
		OutputPath[j--] = 'j';

		j = OutputPath.length();
		while ( OutputPath[j] != '\.' )
		{
			j--;
		}

		OutputPath.insert( j, "SM" );
		s.Process( (char*)InputPath.c_str(), (char*)OutputPath.c_str(), mapRequested, fourierReduction, skin );
		exit(0);
	}

	DIR *dir;
	struct dirent *ent;
	int fileCount = 0;

	dir = opendir (argv[1]);
	while ( ( ent = readdir (dir) ) != NULL ) 
	{
		if ( ent->d_name[0] == '\.' ) 
			continue;

		int i = 0;
		char *result = new char[100];
		char *c = argv[1];
		string fileName;

		while ( c[i] != '\0' )
		{
			fileName += c[i];
			result[i] = c[i];
			i++;
		}
		fileName += '\\';
		result[i] = '\\';
		i++;

		int j = 0;
		while ( ent->d_name[j] != '\0' ) 
		{
			fileName += ent->d_name[j];
			result[i + j] = ent->d_name[j];
			j++;
		}
		int k = fileName.length() - 1;

		bool isJPG = false;
		if ( fileName[k] == 'g' )
			if ( fileName[k - 1] == 'p' )
				if ( fileName[k - 2] == 'j')
					if ( fileName[k - 3] == '\.' )
						isJPG = true;
		bool isTIF = false;
		if ( fileName[k] == 'f')
			if ( fileName[k - 1] == 'i')
				if ( fileName[k - 2] == 't')
					if ( fileName[k - 3] == '\.' )
							isJPG = true;

		if ( isJPG || isTIF )
		{
			fileCount++;
		}
	}
	
	int threadCount = thread::hardware_concurrency();
	c = argv[1];
	i = 0;
	string newFolderName;
	while ( c[i] != '\0' )
	{
		newFolderName += c[i];
		i++;
	}
	newFolderName += "\\SharpnessMap";
	_mkdir( newFolderName.c_str() );
	vector<vector<string>*> fileNames;

	for (int i = 0; i < threadCount; i++)
	{
		fileNames.push_back( new vector<string>() );
	}
	
	dir = opendir ( argv[1] );
	int currentThread = 0;
	while ( ( ent = readdir (dir) ) != NULL ) 
	{
		if ( currentThread == threadCount )
		{
			currentThread = 0;
		}

		if ( ent->d_name[0] == '\.' ) 
			continue;

		int i = 0;
		char *result = new char[100];
		char *c = argv[1];
		string fileName;

		while ( c[i] != '\0' )
		{
			fileName += c[i];
			result[i] = c[i];
			i++;
		}
		fileName += '\\';
		result[i] = '\\';
		i++;

		int j = 0;
		while ( ent->d_name[j] != '\0' ) 
		{
			fileName += ent->d_name[j];
			result[i + j] = ent->d_name[j];
			j++;
		}
		int k = fileName.length() - 1;

		if ( IsJPG( fileName ) || IsTIF( fileName ) )
		{
			fileNames[currentThread]->push_back( fileName );
			currentThread++;
		}
	}

	vector<thread> threads;
	for (int i = 0; i < threadCount; i++)
	{
		threads.push_back( thread( CallSharpnessMap, fileNames[i], mapRequested, fourierReduction, skin ) );
	}


	for (int i = 0; i < threadCount; i++)
	{
		threads[i].join();
	}

	return 0;
}

