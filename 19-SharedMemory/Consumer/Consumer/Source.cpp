#include <Windows.h>


#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>


int main(int argc, char** argv)
{
    //1KB buffers
    DWORD const constBufferSize_ = 1024;

    //Initialized Error Code
    DWORD error = 0xDEADBEEF;

    //Pointer for strtoul argument, no idea if this is best approach but works for now
    char* pInputArgToNumOfBuffers = nullptr;

    //Convert Input Arg to Number of Buffers to allocate
    unsigned long convertednumOfBuffers = strtoul(argv[1], &pInputArgToNumOfBuffers, 10);

    std::vector<HANDLE> vHandles; //i.e Vector of Void Pointers
    std::vector<PVOID> vMapPtrs;  //i.e Vector of Void Pointers

    const std::wstring nameOfSem = L"Global\\MyFileMappingObject_Sync";
    LPCWSTR strSemName = nameOfSem.c_str();

    //Create Counting Semaphore. Each inrement in the count is assocaited with a buffer.  Used to Syncronize Producer/Consumer
    HANDLE handleProcessSync = OpenSemaphore(MAXIMUM_ALLOWED, TRUE, strSemName);

    if (handleProcessSync == 0)
    {
        error = GetLastError();
        std::cout << error << std::endl;
        return -1;
    }

    for (unsigned long i = 0; i < convertednumOfBuffers; i++)
    {

        /////////////////////////////////
       //                              //
       //       Open File Mapping    //
       //                              //
       /////////////////////////////////

        std::unique_ptr <HANDLE> upHandle(new HANDLE);

        const std::wstring nameOfBuffer = L"Global\\MyFileMappingObject_" + std::to_wstring(i);
        LPCWSTR str = nameOfBuffer.c_str();


        //Create Mapped File
        *upHandle.get() = OpenFileMapping(
            FILE_MAP_ALL_ACCESS,   // read/write access
            FALSE,                 // do not inherit the name
            str);

        //Push into Vector
        if (upHandle == 0)
        {
            error = GetLastError();
            std::cout << error << std::endl;
            return -1;
        }

        vHandles.push_back(*upHandle.get()); //Push Handle Value

       /////////////////////////////////
       //                              //
       //        Map View of File      //
       //                              //
       /////////////////////////////////
       {
           //Create Pointer
           std::unique_ptr <PVOID> upBuf(new PVOID);

           //Create Mapped Buffer
           *upBuf.get() = MapViewOfFile(*upHandle.get(),   // handle to map object
               FILE_MAP_ALL_ACCESS, // read/write permission
               0,
               0,
               constBufferSize_);

           //Check Handle
           if (upBuf == 0)
           {
               error = GetLastError();
               std::cout << error << std::endl;
               return -1;
           }

           //Push into Vector
           vMapPtrs.push_back(*upBuf.get()); //Push Handle Value
       }

    }

    //Counter used circle through allocated buffers
    int rollingCtr = 0;

    while (1)
    {
        
        error = WaitForSingleObject(handleProcessSync,INFINITE);

        unsigned char outputBuffer[1024];

        //Copy from Shared Region
        memcpy_s(outputBuffer, 1024, vMapPtrs[rollingCtr], 1024);


        //Data is not cleared, if you want a clean bufer everytime overwrite accordingly


        std::cout << "Output: " << outputBuffer << std::endl;

        if (rollingCtr != convertednumOfBuffers - 1)
        {
            rollingCtr++;
        }
        else
        {
            rollingCtr = 0;
        }
    }



    bool successful = false;

    //Cleanup
    for (unsigned long i = 0; i < convertednumOfBuffers; i++)
    {
        //Unmap Files
        successful = UnmapViewOfFile(vMapPtrs[i]);
        if (false == successful)
        {
            error = GetLastError();
            std::cout << error << std::endl;
            return -1;
        }

        successful = CloseHandle(vHandles[i]);
        if (false == successful)
        {
            error = GetLastError();
            std::cout << error << std::endl;
            return -1;
        }
    }

    successful = CloseHandle(handleProcessSync);
    if (false == successful)
    {
        error = GetLastError();
        std::cout << error << std::endl;
        return -1;
    }
}