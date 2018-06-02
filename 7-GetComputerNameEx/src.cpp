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
	bufCharCount = INFO_BUFFER_SIZE;

	if (!GetComputerNameEx(ComputerNameDnsHostname, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
	_tprintf(TEXT("ComputerNameDnsHostname name:      %s \n"), infoBuf);

	if (!GetComputerNameEx(ComputerNameDnsDomain, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
		_tprintf(TEXT("ComputerNameDnsDomain name:      %s \n"), infoBuf);

	if (!GetComputerNameEx(ComputerNameDnsFullyQualified, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
   _tprintf(TEXT("ComputerNameDnsFullyQualified name:      %s \n"), infoBuf);

	if (!GetComputerNameEx(ComputerNameNetBIOS, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
	_tprintf(TEXT("ComputerNameNetBIOS name:      %s \n"), infoBuf);

	if (!GetComputerNameEx(ComputerNamePhysicalDnsDomain, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
	_tprintf(TEXT("ComputerNamePhysicalDnsDomain name:      %s \n"), infoBuf);

	if (!GetComputerNameEx(ComputerNamePhysicalDnsFullyQualified, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
	_tprintf(TEXT("ComputerNamePhysicalDnsFullyQualified name:      %s \n"), infoBuf);

	if (!GetComputerNameEx(ComputerNamePhysicalDnsHostname, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
		_tprintf(TEXT("ComputerNamePhysicalDnsHostname name:      %s \n"), infoBuf);

	if (!GetComputerNameEx(ComputerNamePhysicalNetBIOS, infoBuf, &bufCharCount))
		_tprintf(TEXT("Error"));
		_tprintf(TEXT("ComputerNamePhysicalNetBIOS name:      %s \n"), infoBuf);
    return 0;
}

