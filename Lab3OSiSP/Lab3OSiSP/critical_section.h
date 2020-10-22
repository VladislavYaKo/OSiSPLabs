#pragma once
#include <windows.h>
#include <xmemory>

typedef struct CriticalSectionElem {
	char sectionName[20];
	bool isCreated = false;
	CRITICAL_SECTION criticalSection;
}CRITICAL_SECTION_ELEM;

HANDLE InitializeFileMapping(const char* fileMappingName);
HANDLE RetrieveFileMapping(const char* fileMappingName);
LPVOID MapView(HANDLE mapFile);
CRITICAL_SECTION_ELEM* CreateCriticalSection(LPVOID fileMapBuf, const char* csName);
CRITICAL_SECTION_ELEM* GetCriticalSection(LPVOID fileMapBuf, const char* csName);
void DeleteCriticalSection(LPVOID fileMapBuf, const char* csName);


