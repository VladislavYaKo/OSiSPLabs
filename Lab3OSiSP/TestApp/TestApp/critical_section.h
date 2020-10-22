#pragma once
#include <winbase.h>
#include <xmemory>

typedef struct CriticalSectionElem {
	char* sectionName = NULL;
	bool isCreated = false;
	CRITICAL_SECTION criticalSection;
}CRITICAL_SECTION_ELEM;

CRITICAL_SECTION_ELEM* CreateCriticalSection(const char* sectionName);
CRITICAL_SECTION_ELEM* GetCriticalSection(const char* sectionName);
void DeleteCriticalSection(const char* sectionName);
void InitializeSharedCS();

