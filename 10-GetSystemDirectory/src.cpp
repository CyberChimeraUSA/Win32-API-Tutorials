// ConsoleApplication5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <tchar.h>
#include <Windows.h>

#define INFO_BUFFER_SIZE 32767
DWORD  bufCharCount = INFO_BUFFER_SIZE;
TCHAR  infoBuf[INFO_BUFFER_SIZE];


int main()
{

	if (!GetSystemDirectory(infoBuf, INFO_BUFFER_SIZE))
		_tprintf(TEXT("GetSystemDirectory"));
	_tprintf(TEXT("\nSystem Directory:   %s \n"), infoBuf);


    return 0;
}

