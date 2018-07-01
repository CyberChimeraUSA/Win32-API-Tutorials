
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
		cout <<"SnapShotHandle: " <<SnapshotHandle << endl;


		void *buf[25];

		PSS_PROCESS_INFORMATION *ptr[500];
		PSS_PROCESS_INFORMATION *ptr2;
		

		DWORD dwQuery = PssQuerySnapshot(SnapshotHandle, PSS_QUERY_PROCESS_INFORMATION, ptr, sizeof(PSS_PROCESS_INFORMATION));
		cout << "PssQuerySnapshot Error Code:" << dwQuery << endl;

		ptr2 = (PSS_PROCESS_INFORMATION *)ptr;


		cout <<"AffinityMask :" <<ptr2->AffinityMask << endl;
		cout <<"BasePriority: " << ptr2->BasePriority << endl;
		cout <<"ExecuteFlags: " << ptr2->ExecuteFlags << endl;
		cout <<"ImageFileName: " <<ptr2->ImageFileName << endl;
		cout <<"PageFaultCount: " << ptr2->PageFaultCount << endl;
		cout <<"PagefileUsage: " << ptr2->PagefileUsage << endl;
		cout <<"ParentProcessId: " << ptr2->ParentProcessId << endl;
		cout <<"PeakPagefileUsage: "<<ptr2->PeakPagefileUsage << endl;
		cout <<"PeakVirtualSize: " <<ptr2->PeakVirtualSize << endl;
		cout <<"PeakWorkingSetSize: "<<ptr2->PeakWorkingSetSize << endl;
		cout <<"PebBaseAddress: "<<ptr2->PebBaseAddress << endl;
		cout <<"PriorityClass: "<<ptr2->PriorityClass << endl;
		cout <<"PrivateUsage: "<<ptr2->PrivateUsage << endl;
		cout <<"QuotaNonPagedPoolUsage: "<<ptr2->QuotaNonPagedPoolUsage << endl;
		cout <<"QuotaPagedPoolUsage: "<<ptr2->QuotaPagedPoolUsage << endl;
		cout <<"QuotaPeakNonPagedPoolUsage: "<<ptr2->QuotaPeakNonPagedPoolUsage << endl;
		cout <<"QuotaPeakPagedPoolUsage: "<<ptr2->QuotaPeakPagedPoolUsage << endl;
		cout << "VirtualSize: "<<ptr2->VirtualSize << endl;
		cout <<"Flags: "<<ptr2->Flags << endl;
		

		PssFreeSnapshot(currentProcess, SnapshotHandle);

		return 0;
	
}

