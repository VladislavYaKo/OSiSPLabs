// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "framework.h"
#include "DatabaseLibrary.h"

const int OPEN_ATTEMPTS = 5;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HRESULT LoadDatabaseFromFile(std::wstring filePath, std::vector<DatabaseRow>& dbRowsList)
{
    std::wifstream inputFile;
    std::wstringstream dbFileText;
    std::wstring dbFileRow;

    for (int i = 0; i < OPEN_ATTEMPTS; i++)
    {
        inputFile.open(filePath, std::ios_base::in | std::ios_base::out);
        if (!inputFile.is_open())
        {
            if (i == OPEN_ATTEMPTS)
                return ERROR_FILE_INVALID;
            Sleep(50);
        }
        else
            break;
    }

    //inputFile >> dbFileText;
    int dbIndex = 0;
    while (std::getline(inputFile, dbFileRow))
    {
        DatabaseRow curRow;
        std::wstringstream tempStream;
        tempStream << dbFileRow;
        curRow.ind = dbIndex++;
        for (int i = 0; i < 8; i++)  //8 - количество полей в структуре без ind
        {
            std::wstring tempStr;
            std::getline(tempStream, tempStr, L' ');
            switch (i)
            {
            case 0:
                curRow.phoneNum = tempStr;
                break;
            case 1:
                curRow.secondName = tempStr;
                break;
            case 2:
                curRow.firstName = tempStr;
                break;
            case 3:
                curRow.patronymic = tempStr;
                break;
            case 4:
                curRow.street = tempStr;
                break;
            case 5:
                try {
                    curRow.houseNum = std::stoi(tempStr.c_str());
                }catch(...)
                { }
                break;
            case 6:
                try
                {
                    curRow.buildingNum = std::stoi(tempStr.c_str());
                }catch(...)
                { }
                break;
            case 7:
                try 
                {
                    curRow.apartmentNum = std::stoi(tempStr.c_str());
                }catch(...)
                { }
                break;
            }
        }  
        dbRowsList.push_back(curRow);
    }

    return S_OK;
}

bool Comparator(ByStringIndex arg1, ByStringIndex arg2)
{
    return arg1.stringField < arg2.stringField;
}

HRESULT MakeupIndexByString(std::vector<DatabaseRow>& dbRowsList, std::vector<ByStringIndex>& reqStringIndex, int indexingFlag)
{
    for (DatabaseRow &dbr : dbRowsList)
    {
        ByStringIndex bsi;
        switch (indexingFlag)
        {
        case INDEXING_BY_SECOND_NAME:
            bsi.stringField = dbr.secondName;
            break;
        case INDEXING_BY_PHONE_NUMBER:
            bsi.stringField = dbr.phoneNum;
            break;
        case INDEXING_BY_STREET:
            bsi.stringField = dbr.street;
            break;
        }
        bsi.rowInd = dbr.ind;
        bsi.correspondRow = &dbr;
        reqStringIndex.push_back(bsi);
    }

    std::sort(reqStringIndex.begin(), reqStringIndex.end(), Comparator);
    switch (indexingFlag)
    {
    case INDEXING_BY_SECOND_NAME:
        secNameIndex = reqStringIndex;
        break;
    case INDEXING_BY_PHONE_NUMBER:
        phoneNumIndex = reqStringIndex;
        break;
    case INDEXING_BY_STREET:
        streetIndex = reqStringIndex;
        break;
    }

    return S_OK;
}

int BinarySearchOneElem(std::vector<ByStringIndex> reqStringIndex, int left, int right, std::wstring key)
{
    int midd = 0;
    while (true)
    {
        midd = (left + right) / 2;

        if (key < reqStringIndex[midd].stringField)       // если искомое меньше значения в ячейке
            right = midd - 1;      // смещаем правую границу поиска
        else if (key > reqStringIndex[midd].stringField)  // если искомое больше значения в ячейке
            left = midd + 1;    // смещаем левую границу поиска
        else                       // иначе (значения равны)
        {            
            return midd;
        }

        if (left > right)          // если границы сомкнулись 
            return -1;
    }
}

std::vector<DatabaseRow> BinarySearch(DatabaseRow dbRow)
{
    /*enum class SearchFieldType
    {
        secondName,
        phoneNum,
        street,
        none,
    };*/
    std::vector<DatabaseRow> result;

    std::vector<ByStringIndex> searchIndex;
    std::wstring key = L"";
    if (dbRow.secondName != L"")
    {
        searchIndex = secNameIndex;
        key = dbRow.secondName;
    }
    else if (dbRow.phoneNum != L"")
    {
        searchIndex = phoneNumIndex;
        key = dbRow.phoneNum;
    }
    else if (dbRow.street != L"")
    {
        searchIndex = streetIndex;
        key = dbRow.street;
    }

    int foundInd;
    if (searchIndex.size() > 0)
        while ((foundInd = BinarySearchOneElem(searchIndex, 0, searchIndex.size() - 1, key)) >= 0)
        {
            result.push_back(*searchIndex[foundInd].correspondRow);
            searchIndex.erase(searchIndex.begin() + foundInd);
        }

    return result;
}


