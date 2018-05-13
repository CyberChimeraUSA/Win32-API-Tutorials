// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <iostream>
using namespace std;
int main()
{
	/*
	
	typedef enum _FIRMWARE_TYPE { 
	  FirmwareTypeUnknown  = 0,
	  FirmwareTypeBios     = 1,
	  FirmwareTypeUefi     = 2,
	  FirmwareTypeMax      = 3
	} FIRMWARE_TYPE, *PFIRMWARE_TYPE;

	*/
	FIRMWARE_TYPE test1;
	
	cout << GetFirmwareType(&test1) << endl;
	

}