#include <Windows.h>


#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>

// Initial Conditions
// argv[1] --> Number of Shared Buffers
// Buffer Size defined as 1024 Bytes

//Notes: Bare Minmuim Error Handling (Developer beware)
//       Apologies for mixing Windows and StandardLib data types
//       

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
   HANDLE handleProcessSync = CreateSemaphore(NULL, 0, convertednumOfBuffers, strSemName);

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
       //       Create File Mapping    //
       //                              //
       /////////////////////////////////

       //Allocate unique pointer
       std::unique_ptr <HANDLE> upHandle(new HANDLE);

       //Create Name
       const std::wstring nameOfBuffer = L"Global\\MyFileMappingObject_" + std::to_wstring(i);
       LPCWSTR str = nameOfBuffer.c_str();


       //Create Mapped File
      *upHandle.get() = CreateFileMapping(
                                   INVALID_HANDLE_VALUE,    // use paging file
                                   NULL,                    // default security
                                   PAGE_READWRITE,          // read/write access
                                   0,                       // maximum object size (high-order DWORD)
                                   constBufferSize_,        // maximum object size (low-order DWORD)
                                   str);     // name of mapping object
      
      //Check Handle
      if (upHandle == 0)
      {
          error = GetLastError();
          std::cout << error << std::endl;
          return -1;
      }

      //Push into Vector
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

       std::string inputString;

       std::cin >> inputString;

       std::cout << "Output: " << inputString << std::endl;

       //Copy into Shared Region
       memcpy_s(vMapPtrs[rollingCtr], 1024, inputString.c_str(),strlen(inputString.c_str()));

       //Tell Consumer Datas Ready
       error = ReleaseSemaphore(handleProcessSync, 1, NULL);
       if (error == -1)
       {
           error = GetLastError();
           std::cout << error << std::endl;
           return -1;
       }

       //Increment or Rollover
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

   return 0;
}