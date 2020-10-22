#include "critical_section.h"
//#pragma section("shared_section", read, write, shared)
const int csArrSize = 10;
//_declspec(allocate("shared_section")) CRITICAL_SECTION_ELEM csElems[csArrSize];

CRITICAL_SECTION_ELEM csElems[csArrSize];
//CRITICAL_SECTION localCS;

int FindReadyCSElem(LPVOID fileMapBuf)
{
	CRITICAL_SECTION_ELEM buf[csArrSize];
	CopyMemory(buf, fileMapBuf, sizeof(buf));

	for (int i = 0; i < csArrSize; i++)
		if (buf[i].isCreated == false)
			return i;

	return -1;
}

HANDLE InitializeFileMapping(const char* fileMappingName)
{
	HANDLE fileMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(csElems), fileMappingName);
	if (fileMap == NULL)
		return NULL;

	LPVOID pBuf = MapViewOfFile(fileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(csElems));
	if (pBuf == NULL)
		return NULL;

	CopyMemory(pBuf, csElems, sizeof(csElems));
	return fileMap;
}

HANDLE RetrieveFileMapping(const char* fileMappingName)
{
	return OpenFileMappingA(FILE_MAP_ALL_ACCESS, TRUE, fileMappingName);
}

LPVOID MapView(HANDLE mapFile)
{
	return MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(csElems));
}

//csName legth should be <=20
CRITICAL_SECTION_ELEM* CreateCriticalSection(LPVOID fileMapBuf, const char *csName)
{
	if (strlen(csName) > 20)
		return NULL;

	CRITICAL_SECTION_ELEM* cse = new CRITICAL_SECTION_ELEM();
	cse->isCreated = true;
	strcpy_s(cse->sectionName, sizeof(cse->sectionName), csName);
	int freeElemIndex = FindReadyCSElem(fileMapBuf);

	CopyMemory(&((CRITICAL_SECTION_ELEM*)fileMapBuf)[freeElemIndex], (PVOID)cse, sizeof(CRITICAL_SECTION_ELEM));
	return cse;
}

CRITICAL_SECTION_ELEM* GetCriticalSection(LPVOID fileMapBuf, const char* csName)
{	
	for (int i = 0; i < csArrSize; i++)
		if (strcmp( ((CRITICAL_SECTION_ELEM*)fileMapBuf)[i].sectionName, csName ) == 0)
		{
			return &((CRITICAL_SECTION_ELEM*)fileMapBuf)[i];
		}

	return NULL;
}

void DeleteCriticalSection(LPVOID fileMapBuf, const char* csName)
{
	for (int i = 0; i < csArrSize; i++)
		if (strcmp( ((CRITICAL_SECTION_ELEM*)fileMapBuf)[i].sectionName, csName ) == 0)
		{	
			strcpy_s( ( ( (CRITICAL_SECTION_ELEM*)fileMapBuf )[i].sectionName), sizeof( ((CRITICAL_SECTION_ELEM*)fileMapBuf)[i].sectionName ), "");
			csElems[i].isCreated = false;
		}
}