#include <iostream>
#include "critical_section.h"
#include <string>

int main()
{
    HANDLE fileMapping =  InitializeFileMapping("FileMapping");

    LPVOID mappingBuf = MapView(fileMapping);

    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    CRITICAL_SECTION_ELEM* critSecElem = CreateSharedCriticalSection(mappingBuf, cs, "crit_sec1");
    CRITICAL_SECTION_ELEM *critSecElemTest = GetSharedCriticalSection(mappingBuf, "crit_sec1");    

    STARTUPINFOA si1;
    PROCESS_INFORMATION pi1;

    STARTUPINFOA si2;
    PROCESS_INFORMATION pi2;

    const char* Proc1Path = "..\\TestApp\\Debug\\TestApp.exe"; 
    const char* Proc2Path = "..\\TestApp2\\Debug\\TestApp2.exe";

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    ZeroMemory(&pi1, sizeof(pi1));

    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    ZeroMemory(&pi2, sizeof(pi2));

    if (!CreateProcessA(Proc1Path, NULL, NULL, NULL, false, NULL, NULL, NULL, &si1, &pi1))
    {
        std::cout << "Couldn't create process 1" << std::endl;
        return 1;
    }

    if (!CreateProcessA(Proc2Path, NULL, NULL, NULL, false, NULL, NULL, NULL, &si2, &pi2))
    {
        std::cout << "Couldn't create process 2" << std::endl;
        return 1;
    }

    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    std::cout << "Created process 1" << std::endl;
    std::cout << "Created process 2" << std::endl;
}
