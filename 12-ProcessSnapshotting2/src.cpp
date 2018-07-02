
#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <psapi.h>
#include <Processsnapshot.h>

using namespace std;
int main()
{

	PSS_CAPTURE_FLAGS  CaptureFlags = (PSS_CAPTURE_FLAGS)PSS_CAPTURE_VA_CLONE
		| PSS_CAPTURE_HANDLES
		| PSS_CAPTURE_HANDLE_NAME_INFORMATION
		| PSS_CAPTURE_HANDLE_BASIC_INFORMATION
		| PSS_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION
		| PSS_CAPTURE_HANDLE_TRACE
		| PSS_CAPTURE_THREADS
		| PSS_CAPTURE_THREAD_CONTEXT
		| PSS_CAPTURE_THREAD_CONTEXT_EXTENDED
		| PSS_CREATE_BREAKAWAY
		| PSS_CREATE_BREAKAWAY_OPTIONAL
		| PSS_CREATE_USE_VM_ALLOCATIONS
		| PSS_CREATE_RELEASE_SECTION;

	HANDLE currentProcess = GetCurrentProcess();
	DWORD currtentProceeID = GetCurrentProcessId();
	HPSS SnapshotHandle;
	PSS_PROCESS_INFORMATION *info_access;
	HMODULE hMod;

	DWORD dwResultCode = PssCaptureSnapshot(currentProcess, CaptureFlags, CONTEXT_ALL, &SnapshotHandle);
	if (dwResultCode != ERROR_SUCCESS)
	{
		cout << "PssCaptureSnapshot Error Code:" << dwResultCode << endl;
		cout << GetLastError() << endl;
		return 1;
	}
	cout << "SnapShotHandle: " << SnapshotHandle << endl;


	void *buf[25];

	PSS_VA_CLONE_INFORMATION *ptr[500];
	PSS_VA_CLONE_INFORMATION *ptr2;

	DWORD dwQuery = PssQuerySnapshot(SnapshotHandle, PSS_QUERY_VA_CLONE_INFORMATION, ptr, sizeof(PSS_VA_CLONE_INFORMATION));
	cout << "PssQuerySnapshot Error Code:" << dwQuery << endl;

	ptr2 = (PSS_VA_CLONE_INFORMATION *)ptr;


	cout << "VaCloneHandle :" << ptr2->VaCloneHandle << endl;
	


	PssFreeSnapshot(currentProcess, SnapshotHandle);

	return 0;

}

