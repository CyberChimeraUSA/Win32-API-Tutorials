//https://docs.microsoft.com/en-us/windows/win32/procthread/creating-processes

//Citation -> Pre-Modified Original Example - http://www.debuginfo.com/examples/dbgexamples.html


#include <windows.h>
#include <stdio.h>
#include <inttypes.h>

//x86/x64 Processor Flags
#define UE_TRAP_FLAG (0x100)
#define UE_RESUME_FLAG (0x10000)

//Macros for Thread Manupulation
#define THREAD_GETSETSUSPEND (THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION)

//Global Bool (Sloppy)
bool trapTracker = true;
HANDLE GlobalhThread_;


//Function Declarations
bool DebugLoop(DWORD Timeout = INFINITE);
void ReportCreateProcessEvent(DWORD ProcessId, DWORD ThreadId, const CREATE_PROCESS_DEBUG_INFO& Event);
void ReportExitProcessEvent(DWORD ProcessId, DWORD ThreadId, const EXIT_PROCESS_DEBUG_INFO& Event);
void ReportCreateThreadEvent(DWORD ProcessId, DWORD ThreadId, const CREATE_THREAD_DEBUG_INFO& Event);
void ReportExitThreadEvent(DWORD ProcessId, DWORD ThreadId, const EXIT_THREAD_DEBUG_INFO& Event);
void ReportLoadDllEvent(DWORD ProcessId, DWORD ThreadId, const LOAD_DLL_DEBUG_INFO& Event);
void ReportUnloadDllEvent(DWORD ProcessId, DWORD ThreadId, const UNLOAD_DLL_DEBUG_INFO& Event);
void ReportExceptionEvent(DWORD ProcessId, DWORD ThreadId, const EXCEPTION_DEBUG_INFO& Event);
void ReportDebugStringEvent(DWORD ProcessId, DWORD ThreadId, const OUTPUT_DEBUG_STRING_INFO& Event);
void ReportRipEvent(DWORD ProcessId, DWORD ThreadId, const RIP_INFO& Event);
void ReportTimeout(DWORD Timeout);
bool EnableDebugPrivilege(bool Enable);

using namespace std;


int main(int argc, char** argv)
{
	
	//Structe Allocation for CreateProcess()
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	//Initalize Structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//Turn on Debugger for Current Process
	EnableDebugPrivilege(true);

	//Taks an Argument for Process of Choice, Using Debug Options in Build Properties
	if (argc != 2)
	{
		printf("Usage: %s [cmdline]\n", argv[0]);
		return 1;
	}

	// Start the child process. 
	if (!CreateProcessA(NULL,   // No module name (use command line)
		argv[1],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS,             
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 1;
	}

	//Debug Prints
	printf("  *ProcessId*:                %u\n", pi.dwProcessId);
	printf("  *ThreadId*:                 %u\n", pi.hThread);

	//Infinite Loop
	DebugLoop(INFINITE);

	 // Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}

bool EnableDebugPrivilege(bool Enable)
{
	bool Success = false;

	HANDLE hToken = NULL;

	DWORD ec = 0;

	do
	{
		// Open the process' token

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			ec = GetLastError();
			printf("OpenProcessToken() failed.");
			break;
		}


		// Lookup the privilege value 

		TOKEN_PRIVILEGES tp;

		tp.PrivilegeCount = 1;

		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			ec = GetLastError();
			printf("LookupPrivilegeValue() failed.");
			break;
		}


		// Enable/disable the privilege

		tp.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
		{
			ec = GetLastError();
			printf("AdjustPrivilegeValue() failed.");
			break;
		}


		// Success 

		Success = true;

	} while (0);


	// Cleanup

	if (hToken != NULL)
	{
		if (!CloseHandle(hToken))
		{
			ec = GetLastError();
			printf("CloseHandle() failed.");
		}
	}


	// Complete 

	return Success;

}

bool DebugLoop(DWORD Timeout)
{
	// Run the debug loop and handle the events 

	DEBUG_EVENT DebugEvent;

	bool bContinue = true;

	bool bSeenInitialBreakpoint = false;

	while (bContinue)
	{
		// Call WaitForDebugEvent
		if (WaitForDebugEvent(&DebugEvent, Timeout))
		{
			// Handle the debug event 

			DWORD ContinueStatus = DBG_CONTINUE;

			switch (DebugEvent.dwDebugEventCode)
			{


				/************************/
				/*Event - Process Create*/
				/************************/
			case CREATE_PROCESS_DEBUG_EVENT:

				ReportCreateProcessEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DebugEvent.u.CreateProcessInfo);

				{

					//HANDLE hProcess2_ = DebugEvent.u.CreateProcessInfo.hProcess;
					GlobalhThread_ = DebugEvent.u.CreateProcessInfo.hProcess;

					HANDLE hFile = DebugEvent.u.CreateProcessInfo.hFile;

					if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
					{
						if (!CloseHandle(hFile))
						{
							printf("CloseHandle(hFile) failed. Error: %u\n", GetLastError());
							//_ASSERTE(!_T("CloseHandle(hFile) failed."));
						}
					}

				}
				break;




				/************************/
				/*Event - Process Exit  */
				/************************/
			case EXIT_PROCESS_DEBUG_EVENT:
				ReportExitProcessEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DebugEvent.u.ExitProcess);
				bContinue = false; // Last event - exit the loop
				break;




				/************************/
				/*Event - Thread Create */
				/************************/
			case CREATE_THREAD_DEBUG_EVENT:
				ReportCreateThreadEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DebugEvent.u.CreateThread);

				break;


				/************************/
				/*Event - Thread Exit   */
				/************************/
			case EXIT_THREAD_DEBUG_EVENT:
				ReportExitThreadEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DebugEvent.u.ExitThread);
				break;


				/************************/
				/*Event - Load DLL      */
				/************************/
			case LOAD_DLL_DEBUG_EVENT:
				ReportLoadDllEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DebugEvent.u.LoadDll);

				{
					HANDLE hFile = DebugEvent.u.LoadDll.hFile;

					if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
					{
						if (!CloseHandle(hFile))
						{
							printf("CloseHandle(hFile) failed. Error: %u\n", GetLastError());
							//_ASSERTE(!_T("CloseHandle(hFile) failed."));
						}
					}
				}
				break;

				/************************/
				/*Event - Unload DLL    */
				/************************/
			case UNLOAD_DLL_DEBUG_EVENT:
				ReportUnloadDllEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
					DebugEvent.u.UnloadDll);
				break;


			case OUTPUT_DEBUG_STRING_EVENT:
				ReportDebugStringEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
					DebugEvent.u.DebugString);
				break;

			case RIP_EVENT:
				ReportRipEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
					DebugEvent.u.RipInfo);
				break;


			case EXCEPTION_DEBUG_EVENT:
				ReportExceptionEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DebugEvent.u.Exception);
				bool tmo;

				// By default, do not handle the exception 
				// (let the debuggee handle it if it wants to)

				ContinueStatus = DBG_EXCEPTION_NOT_HANDLED;

				// Now the special case - the initial breakpoint 


				CONTEXT myDBGContext;

				//Open Thread
				HANDLE hThread_ = OpenThread(THREAD_GETSETSUSPEND, false, DebugEvent.dwThreadId);

				//Sloppy Handlers
				if (NULL == hThread_)
				{
					ContinueStatus = DBG_CONTINUE;
					break;
				}

				//Full Control of Thread
				myDBGContext.ContextFlags = CONTEXT_ALL;

				//Grabs Current Thread Conte
				tmo = GetThreadContext(hThread_, &myDBGContext);

				//Sloppy Handlers
				if (false == tmo)
				{
					ContinueStatus = DBG_CONTINUE;
					break;
				} 

				//Debug Print- Shows Changing IP
				printf("   rip read : 0x%" PRIx64 "\n", myDBGContext.Rip);

				//Sets Trap Flag
				if (trapTracker)
				{
					//Writes Trap Bits to EFLAGS Register 
					myDBGContext.EFlags |= UE_TRAP_FLAG;
					//Sets Current Contexts to thread 
					tmo = SetThreadContext(hThread_, &myDBGContext);

					trapTracker = false;

					//Tell Program to continue, will Fail to exception if not set properly
					ContinueStatus = DBG_CONTINUE;

				}

				//Unsets Trap Flag
				else if (!trapTracker)
				{
					myDBGContext.EFlags &= ~UE_TRAP_FLAG;
					tmo = SetThreadContext(hThread_, &myDBGContext);
					ContinueStatus = DBG_CONTINUE;
				}

				CloseHandle(hThread_);


				DWORD ExceptionCode = DebugEvent.u.Exception.ExceptionRecord.ExceptionCode;

				if (!bSeenInitialBreakpoint && (ExceptionCode == EXCEPTION_BREAKPOINT))
				{
					ContinueStatus = DBG_CONTINUE;
					bSeenInitialBreakpoint = true;
				}
				break;
			}

			// Let the debuggee continue 

			if (!ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, ContinueStatus))
			{
				printf("ContinueDebugEvent() failed. Error: %u \n", GetLastError());
				//_ASSERTE(!_T("ContinueDebugEvent() failed."));
				return false;
			}


			// Proceed to the beginning of the loop...

		}
		else
		{
			// WaitForDebugEvent failed...

			// Is it because of timeout ?

			DWORD ErrCode = GetLastError();

			if (ErrCode == ERROR_SEM_TIMEOUT)
			{
				// Yes, report timeout and continue 
				ReportTimeout(Timeout);
			}
			else
			{
				// No, exit the loop
				printf("WaitForDebugEvent() failed. Error: %u \n", GetLastError());
				//_ASSERTE(!_T("WaitForDebugEvent() failed."));
				return false;
			}
		}
	}


	// Complete 

	return true;

}




void ReportCreateProcessEvent(DWORD ProcessId, DWORD ThreadId, const CREATE_PROCESS_DEBUG_INFO& Event)
{
	printf("EVENT: Process creation\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  CREATE_PROCESS_DEBUG_INFO members:\n");
	printf("    hFile:                  %08p\n", Event.hFile);
	printf("    hProcess:               %08p\n", Event.hProcess);
	printf("    hThread                 %08p\n", Event.hThread);
	printf("    lpBaseOfImage:          %08p\n", Event.lpBaseOfImage);
	printf("    dwDebugInfoFileOffset:  %08x\n", Event.dwDebugInfoFileOffset);
	printf("    nDebugInfoSize:         %08x\n", Event.nDebugInfoSize);
	printf("    lpThreadLocalBase:      %08p\n", Event.lpThreadLocalBase);
	printf("    lpStartAddress:         %08p\n", Event.lpStartAddress);
	printf("    lpImageName:            %08p\n", Event.lpImageName);
	printf("    fUnicode:               %u\n", Event.fUnicode);

}

void ReportExitProcessEvent(DWORD ProcessId, DWORD ThreadId, const EXIT_PROCESS_DEBUG_INFO& Event)
{
	printf("EVENT: Process exit\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  EXIT_PROCESS_DEBUG_INFO members:\n");
	printf("    dwExitCode:             %u\n", Event.dwExitCode);

}

void ReportCreateThreadEvent(DWORD ProcessId, DWORD ThreadId, const CREATE_THREAD_DEBUG_INFO& Event)
{
	printf("EVENT: Thread creation\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  CREATE_THREAD_DEBUG_INFO members:\n");
	printf("    hThread                 %08p\n", Event.hThread);
	printf("    lpThreadLocalBase:      %08p\n", Event.lpThreadLocalBase);
	printf("    lpStartAddress:         %08p\n", Event.lpStartAddress);

}

void ReportExitThreadEvent(DWORD ProcessId, DWORD ThreadId, const EXIT_THREAD_DEBUG_INFO& Event)
{
	printf("EVENT: Thread exit\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  EXIT_THREAD_DEBUG_INFO members:\n");
	printf("    dwExitCode:             %u\n", Event.dwExitCode);

}

void ReportLoadDllEvent(DWORD ProcessId, DWORD ThreadId, const LOAD_DLL_DEBUG_INFO& Event)
{
	printf("EVENT: DLL loaded\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  LOAD_DLL_DEBUG_INFO members:\n");
	printf("    hFile:                  %08p\n", Event.hFile);
	printf("    lpBaseOfDll:            %08p\n", Event.lpBaseOfDll);
	printf("    dwDebugInfoFileOffset:  %08x\n", Event.dwDebugInfoFileOffset);
	printf("    nDebugInfoSize:         %08x\n", Event.nDebugInfoSize);
	printf("    lpImageName:            %08p\n", Event.lpImageName);
	printf("    fUnicode:               %u\n", Event.fUnicode);

}

void ReportUnloadDllEvent(DWORD ProcessId, DWORD ThreadId, const UNLOAD_DLL_DEBUG_INFO& Event)
{
	printf("EVENT: DLL unloaded\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  UNLOAD_DLL_DEBUG_INFO members:\n");
	printf("    lpBaseOfDll:            %08p\n", Event.lpBaseOfDll);

}

void ReportExceptionEvent(DWORD ProcessId, DWORD ThreadId, const EXCEPTION_DEBUG_INFO& Event)
{
	printf("EVENT: Exception\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  EXCEPTION_DEBUG_INFO members:\n");
	printf("    dwFirstChance:          %u\n", Event.dwFirstChance);
	printf("    EXCEPTION_RECORD members:\n");
	printf("      ExceptionCode:        %08x\n", Event.ExceptionRecord.ExceptionCode);
	printf("      ExceptionFlags:       %08x\n", Event.ExceptionRecord.ExceptionFlags);
	printf("      ExceptionRecord:      %08p\n", Event.ExceptionRecord.ExceptionRecord);
	printf("      ExceptionAddress:     %08p\n", Event.ExceptionRecord.ExceptionAddress);
	printf("      NumberParameters:     %u\n", Event.ExceptionRecord.NumberParameters);

	DWORD NumParameters = Event.ExceptionRecord.NumberParameters;

	if (NumParameters > EXCEPTION_MAXIMUM_PARAMETERS)
		NumParameters = EXCEPTION_MAXIMUM_PARAMETERS;

	for (DWORD i = 0; i < NumParameters; i++)
		printf("      ExceptionInformation[%d]:     %08p\n", i, Event.ExceptionRecord.ExceptionInformation[i]);

}

void ReportDebugStringEvent(DWORD ProcessId, DWORD ThreadId, const OUTPUT_DEBUG_STRING_INFO& Event)
{
	printf("EVENT: Debug string\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  OUTPUT_DEBUG_STRING_INFO members:\n");
	printf("    lpDebugStringData:      %08p\n", Event.lpDebugStringData);
	printf("    fUnicode:               %u\n", Event.fUnicode);
	printf("    nDebugStringLength:     %u\n", Event.nDebugStringLength);

}

void ReportRipEvent(DWORD ProcessId, DWORD ThreadId, const RIP_INFO& Event)
{
	printf("EVENT: RIP\n");

	printf("  ProcessId:                %u\n", ProcessId);
	printf("  ThreadId:                 %u\n", ThreadId);

	printf("  RIP_INFO members:\n");
	printf("    dwError:                %08x\n", Event.dwError);
	printf("    dwType:                 %08x\n", Event.dwType);

}

void ReportTimeout(DWORD Timeout)
{
	printf("TIMEOUT: %u milliseconds\n", Timeout);
}