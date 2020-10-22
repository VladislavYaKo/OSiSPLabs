#include <iostream>
#include "critical_section.h"

int main()
{
    //InitializeSharedCS();

    //CRITICAL_SECTION_ELEM *testCS = CreateCriticalSection("test");

    HANDLE fileMapping =  InitializeFileMapping("FileMapping");

    LPVOID mappingBuf = MapView(fileMapping);

    CRITICAL_SECTION_ELEM* critSecElem = CreateCriticalSection(mappingBuf, "crit_sec1");

    int debug = 11;
}
