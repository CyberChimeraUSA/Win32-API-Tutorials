// Win32Project2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project4.h"
#include <stdio.h>
#define BUF_SIZE 256

//wind32 main
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	CHAR buffer[BUF_SIZE];

	//opens 1st text file
	hIn = CreateFile(L".\\a.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open input file \n");
		return 2;
	}
	//opens 2nd text file
	hOut = CreateFile(L".\\b.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open output file \n");
		return 3;
	}
	//takes input file, copies to output
	while (ReadFile(hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0)
	{
		WriteFile(hOut, buffer, nIn, &nOut, NULL);
		if (nIn != nOut) {
			printf("Fatal Error \n");
			return 4;
		}
	}
	CloseHandle(hIn);
	CloseHandle(hOut);
	return 0;
}


