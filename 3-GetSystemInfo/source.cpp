// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;

int main()
{
	SYSTEM_INFO sSysInfo;
	DWORD     dwPageSize;
	LPVOID    lpMinimumApplicationAddress;
	LPVOID    lpMaximumApplicationAddress;
	DWORD_PTR dwActiveProcessorMask;
	DWORD     dwNumberOfProcessors;
	DWORD     dwProcessorType;
	DWORD     dwAllocationGranularity;
	WORD      wProcessorLevel;
	WORD      wProcessorRevision;

	GetSystemInfo(&sSysInfo);
	cout << "dwPageSize: " << sSysInfo.dwPageSize << endl;
	cout << "lpMinimumApplicationAddress: " << sSysInfo.lpMinimumApplicationAddress << endl;
	cout << "lpMaximumApplicationAddress: " << sSysInfo.lpMaximumApplicationAddress << endl;
	cout << "dwActiveProcessorMask: " << sSysInfo.dwActiveProcessorMask << endl;
	cout << "dwNumberOfProcessors: " << sSysInfo.dwNumberOfProcessors << endl;
	cout << "dwProcessorType: " << sSysInfo.dwProcessorType << endl;
	cout << "dwAllocationGranularity: " << sSysInfo.dwAllocationGranularity << endl;
	cout << "wProcessorLevel: " << sSysInfo.wProcessorLevel << endl;
	cout << "wProcessorRevision: " << sSysInfo.wProcessorRevision << endl;

    return 0;
}

