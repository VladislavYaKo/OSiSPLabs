﻿#include <iostream>
#include <windows.h>
#include "../../Lab3OSiSP/critical_section.h"

int main()
{
    HANDLE fileMapping = RetrieveFileMapping("FileMapping");

    if (fileMapping == NULL)
    {
        DWORD err = GetLastError();
        std::cout << "Cannot create map file. Error code: " << err << std::endl;
        return -1;
    }

    LPVOID mappingBuf = MapView(fileMapping);

    if (mappingBuf == NULL)
    {
        DWORD err = GetLastError();
        std::cout << "Cannot create mapping buf" << err << std::endl;
        return -1;
    }

    CRITICAL_SECTION_ELEM* critSecElem = GetCriticalSection(mappingBuf, "crit_sec1");

    int debug = 11;

    if (critSecElem != NULL)
    {
        if (TryEnterCriticalSection(&critSecElem->criticalSection))
        {
            std::cout << "TestApp2. Critical section retrieved, name: " << critSecElem->sectionName << ". Owning thread address: " << critSecElem->criticalSection.OwningThread << std::endl;
            LeaveCriticalSection(&critSecElem->criticalSection);
        }
        else
            std::cout << "TestApp2: cannot enter critical section" << std::endl;
    }
    else
        std::cout << "Critical section null pointer" << std::endl;

    return 1;
}

