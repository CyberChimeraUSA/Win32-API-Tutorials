/*
Libs 

kernel32.lib
user32.lib
gdi32.lib
winspool.lib
comdlg32.lib
advapi32.lib
shell32.lib
ole32.lib
oleaut32.lib
uuid.lib
odbc32.lib
odbccp32.lib
Ws2_32.lib
*/

/*
_CRT_SECURE_NO_WARNINGS
WIN32_LEAN_AND_MEAN
WIN32
_DEBUG
_WINDOWS
UNICODE
_UNICODE
_CONSOLE
*/
#include "pch.h"
#include <iostream>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <Ws2tcpip.h>
#include <string>

#define MAX_ADDRESS_STRING_LENGTH   64


typedef struct _QueryContext
{
	OVERLAPPED      QueryOverlapped;
	PADDRINFOEX     QueryResults;
	HANDLE          CompleteEvent;
}QUERY_CONTEXT, *PQUERY_CONTEXT;

using namespace std;

VOID WINAPI QueryCompleteCallback(_In_ DWORD Error, _In_ DWORD Bytes, _In_ LPOVERLAPPED Overlapped)
{
	PQUERY_CONTEXT  QueryContext = NULL;
	PADDRINFOEX     QueryResults = NULL;
	WCHAR           AddrString[MAX_ADDRESS_STRING_LENGTH];
	DWORD           AddressStringLength;

	UNREFERENCED_PARAMETER(Bytes);

	QueryContext = CONTAINING_RECORD(Overlapped, QUERY_CONTEXT, QueryOverlapped);
	if (Error != ERROR_SUCCESS)
	{
		wprintf(L"ResolveName failed with %d\n", Error);
		goto exit;
	}

	wprintf(L"ResolveName succeeded. Query Results:\n");

	QueryResults = QueryContext->QueryResults;

	while (QueryResults)
	{
		AddressStringLength = MAX_ADDRESS_STRING_LENGTH;

		WSAAddressToString(QueryResults->ai_addr, (DWORD)QueryResults->ai_addrlen, NULL,
			AddrString,
			&AddressStringLength);

		wprintf(L"Ip Address: %s\n", AddrString);
		QueryResults = QueryResults->ai_next;
	}

exit:

	if (QueryContext->QueryResults)
	{
		FreeAddrInfoEx(QueryContext->QueryResults);
	}

	SetEvent(QueryContext->CompleteEvent);
	return;

}

int main()
{
	INT                 Error = ERROR_SUCCESS;
	WSADATA             wsaData;
	BOOL                IsWSAStartupCalled = FALSE;
	ADDRINFOEX          Hints;
	QUERY_CONTEXT       QueryContext;
	HANDLE              CancelHandle = NULL;
	DWORD               QueryTimeout = 5 * 1000;
	char input[256] = { 0 };
	wchar_t converted_input[128] = { 0 };

	ZeroMemory(&QueryContext, sizeof(QueryContext));
	Error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (Error != 0)
	{
		goto exit;
	}

	IsWSAStartupCalled = TRUE;

	ZeroMemory(&Hints, sizeof(Hints));
	Hints.ai_family = AF_UNSPEC;


	QueryContext.CompleteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (QueryContext.CompleteEvent == NULL)
	{
		Error = GetLastError();
		wprintf(L"Failed to create completion event: Error %d\n", Error);
		goto exit;
	}

	cout << "Enter Hostname" << "\n";
	cin >> input;
	mbstowcs(converted_input, input, strlen(input) + 1);//Plus null

	Error = GetAddrInfoExW(converted_input, NULL, NS_DNS, NULL, &Hints, &QueryContext.QueryResults, NULL, &QueryContext.QueryOverlapped, QueryCompleteCallback, &CancelHandle);

	if (Error != WSA_IO_PENDING)
	{
		QueryCompleteCallback(Error, 0, &QueryContext.QueryOverlapped);
		goto exit;
	}

	if (WaitForSingleObject(QueryContext.CompleteEvent, QueryTimeout) == WAIT_TIMEOUT)
	{
		wprintf(L"The query took longer than %d seconds to complete cancelling the query...\n", QueryTimeout / 1000);
		GetAddrInfoExCancel(&CancelHandle);
		WaitForSingleObject(QueryContext.CompleteEvent, INFINITE);
	}

exit:
	if (IsWSAStartupCalled)
	{
		WSACleanup();
	}

	if (QueryContext.CompleteEvent)
	{
		CloseHandle(QueryContext.CompleteEvent);
	}

	return Error;

}


