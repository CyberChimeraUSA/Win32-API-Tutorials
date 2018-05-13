// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>


using namespace std;

int main(){

	HW_PROFILE_INFO   HwProfInfo;

	if (!GetCurrentHwProfile(&HwProfInfo))
	{
		_tprintf(TEXT("GetCurrentHwProfile failed with error %lx\n"),
			GetLastError());
		return 1;
	}
	_tprintf(TEXT("DockInfo = %d\n"), HwProfInfo.dwDockInfo);
	_tprintf(TEXT("Profile Guid = %s\n"), HwProfInfo.szHwProfileGuid);
	_tprintf(TEXT("Friendly Name = %s\n"), HwProfInfo.szHwProfileName);

	return 0;
}

