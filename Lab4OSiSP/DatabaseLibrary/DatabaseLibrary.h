#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#define DllExport extern "C" __declspec(dllexport)

#define INDEXING_BY_SECOND_NAME 1
#define INDEXING_BY_PHONE_NUMBER 2
#define INDEXING_BY_STREET 3
//EXPORT HRESULT CALLBACK LoadDatabaseFromFile(const std::string fileName, std::vector<DatabaseRow> dbRowsList);

struct /*__declspec(dllexport)*/ DatabaseRow
{
	int ind;
	std::wstring phoneNum;
	std::wstring secondName;
	std::wstring firstName;
	std::wstring patronymic;
	std::wstring street;
	int houseNum;
	int buildingNum;
	int apartmentNum;
};

struct ByStringIndex
{
	std::wstring stringField;
	int rowInd;
	DatabaseRow* correspondRow;
};

std::vector<DatabaseRow> dbRowsList = {};
std::vector<ByStringIndex> secNameIndex, phoneNumIndex, streetIndex;

DllExport HRESULT LoadDatabaseFromFile(const std::wstring filePath, std::vector<DatabaseRow>& dbRowsList);
DllExport HRESULT MakeupIndexByString(std::vector<DatabaseRow>& dbRowsList, std::vector<ByStringIndex>& reqStringIndex, int indexingFlag);
int BinarySearchOneElem(std::vector<ByStringIndex> reqStringIndex, int left, int right, std::wstring key);
DllExport std::vector<DatabaseRow> BinarySearch(DatabaseRow dbRow);