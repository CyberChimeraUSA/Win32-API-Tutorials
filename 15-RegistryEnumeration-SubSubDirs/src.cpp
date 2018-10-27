// DebugApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <tchar.h>
#include <pdh.h>
#include <string>
#include <stdio.h>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define _CRT_SECURE_NO_WARNINGS

//Not Optomized
int list_subkeys(TCHAR *val);
int list_subsubkeys(TCHAR *val);


using namespace std;
int main(int argc, char* argv[])
{
	HKEY hTestKey;
	DWORD i, retCode;
	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, NULL, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		//QueryKey(hTestKey);
		TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		DWORD    cbName;                   // size of name string 
		TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys = 0;               // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;              // number of values for key 
		DWORD    cchMaxValue;          // longest value name 
		DWORD    cbMaxValueData;       // longest value data 
		DWORD    cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime;      // last write time 

		retCode = RegQueryInfoKey(hTestKey, achClass, &cchClassName, NULL, &cSubKeys,
			&cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue
			, &cbMaxValueData, &cbSecurityDescriptor, &ftLastWriteTime);

		if (cSubKeys)
		{
			printf("\nNumber of subkeys: %d\n", cSubKeys);

			for (i = 0; i < cSubKeys; i++)
			{
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyEx(hTestKey, i, achKey, &cbName, NULL, 
					NULL, NULL, &ftLastWriteTime);
				if (retCode == ERROR_SUCCESS)
				{
					_tprintf(TEXT("(%d) %s\n"), i + 1, achKey);
					list_subkeys(achKey);  //12 in HKEY_CURRENT_USER
				}
			}
		}

	}
	RegCloseKey(hTestKey);

	return 0;
}

int list_subkeys(TCHAR *val)
{
	HKEY hTestKey;
	DWORD i, retCode;
	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;
	TCHAR    concat_key[MAX_KEY_LENGTH];
	CHAR    path_slash[2];

	if (RegOpenKeyEx(HKEY_CURRENT_USER, val, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		//QueryKey(hTestKey);
		TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		DWORD    cbName;                   // size of name string 
		TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys = 0;               // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;              // number of values for key 
		DWORD    cchMaxValue;          // longest value name 
		DWORD    cbMaxValueData;       // longest value data 
		DWORD    cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime;      // last write time 

		retCode = RegQueryInfoKey(hTestKey, achClass, &cchClassName, NULL, &cSubKeys,
			&cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue
			, &cbMaxValueData, &cbSecurityDescriptor, &ftLastWriteTime);

		if (cSubKeys)
		{
			printf("   Number of subkeys_1: %d\n", cSubKeys);

			if (cSubKeys == 99)
			{
				printf("Break \n");
			}

			for (i = 0; i < cSubKeys; i++)
			{
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyEx(hTestKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
				if (retCode == ERROR_SUCCESS)
				{
					_tprintf(TEXT("   (%d) %s\n"), i + 1, achKey);
					_tcscpy_s(concat_key, val);
					 _tcscat_s(concat_key, TEXT("\\"));
					 _tcscat_s(concat_key, achKey);
					 _tprintf(TEXT("    %s\n"), concat_key);
					 list_subsubkeys(concat_key);
				}
			}
		}


	}
	RegCloseKey(hTestKey);
	printf("\n");
	return 0;
}

int list_subsubkeys(TCHAR *val)
{
	HKEY hTestKey;
	DWORD i, retCode;
	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, val, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		//QueryKey(hTestKey);
		TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		DWORD    cbName;                   // size of name string 
		TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys = 0;               // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;              // number of values for key 
		DWORD    cchMaxValue;          // longest value name 
		DWORD    cbMaxValueData;       // longest value data 
		DWORD    cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime;      // last write time 

		retCode = RegQueryInfoKey(hTestKey, achClass, &cchClassName, NULL, &cSubKeys,
			&cbMaxSubKey, &cchMaxClass, &cValues, &cchMaxValue
			, &cbMaxValueData, &cbSecurityDescriptor, &ftLastWriteTime);

		if (cSubKeys)
		{
			printf("      Number of subsubkeys_2: %d\n", cSubKeys);

			for (i = 0; i < cSubKeys; i++)
			{
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyEx(hTestKey, i, achKey, &cbName, NULL,
					NULL, NULL, &ftLastWriteTime);
				if (retCode == ERROR_SUCCESS)
				{
					_tprintf(TEXT("      (%d) %s\n"), i + 1, achKey);
					//list_subkeys(achKey);  //12 in HKEY_CURRENT_USER
				}
			}
		}

	}
	RegCloseKey(hTestKey);

	return 0;
	printf("\n");
	return 0;
}