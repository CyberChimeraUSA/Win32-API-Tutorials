
#include "stdafx.h"
#include "windows.h"


#define BUF_SIZE 0x200
static VOID UnixLikeCat(HANDLE, HANDLE);
int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hInFile, hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL dashS;
	int iArg, iFirstFile;

	hInFile = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	UnixLikeCat(hInFile, hStdOut);
	CloseHandle(hInFile);

}

static VOID UnixLikeCat(HANDLE hInFile, HANDLE hOutFile)
{
	DWORD nIn, nOut;
	BYTE buffer[BUF_SIZE];

	while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL) && (nIn != 0)
		&& WriteFile(hOutFile, buffer, nIn, &nOut, NULL));

	return;
}



