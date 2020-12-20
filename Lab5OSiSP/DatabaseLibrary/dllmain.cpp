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

DatabaseRow GetDBRowFromText(std::wstringstream& srcText, unsigned int index)
{
    DatabaseRow result;
    result.ind = index;
    for (int i = 0; i < 8; i++)  //8 - количество полей в структуре без ind
    {
        std::wstring tempStr;
        std::getline(srcText, tempStr, L' ');
        switch (i)
        {
        case 0:
            result.phoneNum = tempStr;
            break;
        case 1:
            result.secondName = tempStr;
            break;
        case 2:
            result.firstName = tempStr;
            break;
        case 3:
            result.patronymic = tempStr;
            break;
        case 4:
            result.street = tempStr;
            break;
        case 5:
            try {
                result.houseNum = std::stoi(tempStr.c_str());
            }
            catch (...)
            {
            }
            break;
        case 6:
            try
            {
                result.buildingNum = std::stoi(tempStr.c_str());
            }
            catch (...)
            {
            }
            break;
        case 7:
            try
            {
                result.apartmentNum = std::stoi(tempStr.c_str());
            }
            catch (...)
            {
            }
            break;
        }
    }
    return result;
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

    int dbIndex = 0;
    while (std::getline(inputFile, dbFileRow))
    {
        std::wstringstream tempStream;
        tempStream << dbFileRow;
        dbRowsList.push_back(GetDBRowFromText(tempStream, dbIndex++));       
    }

    inputFile.close();
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

std::vector<DatabaseRow> LinearSearchByField(std::vector<DatabaseRow> src, FieldType fieldType, std::wstring strKey = L"", int intKey = -1)
{
    std::vector<DatabaseRow> result = {};
    for (int i = 0; i < src.size(); i++)
    {
        if (strKey != L"")
            switch (fieldType)
            {
            case FieldType::phoneNum:
                if (src[i].phoneNum == strKey)
                    result.push_back(src[i]);
                break;
            case FieldType::secondName:
                if (src[i].secondName == strKey)
                    result.push_back(src[i]);
                break;
            case FieldType::firstName:
                if (src[i].firstName == strKey)
                    result.push_back(src[i]);
                break;
            case FieldType::patronymic:
                if (src[i].patronymic == strKey)
                    result.push_back(src[i]);
                break;
            case FieldType::street:
                if (src[i].street == strKey)
                    result.push_back(src[i]);
                break;
            }
        else if(intKey >= 0)
            switch (fieldType)
            {
            case FieldType::houseNum:
                if (src[i].houseNum == intKey)
                    result.push_back(src[i]);
                break;
            case FieldType::buildingNum:
                if (src[i].buildingNum == intKey)
                    result.push_back(src[i]);
                break;
            case FieldType::apartmentNum:
                if (src[i].apartmentNum == intKey)
                    result.push_back(src[i]);
                break;
            }
    }

    return result;
}

std::vector<DatabaseRow> LinearSearch(std::vector<DatabaseRow> db, DatabaseRow dbRow)
{
    std::vector<DatabaseRow> result = {};
    std::vector<DatabaseRow> buf = db;
    if (dbRow.phoneNum != L"")
        buf = LinearSearchByField(buf, FieldType::phoneNum, dbRow.phoneNum);
    if (dbRow.secondName != L"")
        buf = LinearSearchByField(buf, FieldType::secondName, dbRow.secondName);
    if (dbRow.firstName != L"")
        buf = LinearSearchByField(buf, FieldType::firstName, dbRow.firstName);
    if (dbRow.patronymic != L"")
        buf = LinearSearchByField(buf, FieldType::patronymic, dbRow.patronymic);
    if (dbRow.street != L"")
        buf = LinearSearchByField(buf, FieldType::street, dbRow.street);
    if (dbRow.houseNum >= 0)
        buf = LinearSearchByField(buf, FieldType::houseNum, L"", dbRow.houseNum);
    if (dbRow.apartmentNum >= 0)
        buf = LinearSearchByField(buf, FieldType::apartmentNum, L"", dbRow.apartmentNum);
    if (dbRow.buildingNum >= 0)
        buf = LinearSearchByField(buf, FieldType::buildingNum, L"", dbRow.buildingNum);
    result.insert(std::end(result), std::begin(buf), std::end(buf));

    return result;
}

std::wstring trim(std::wstring src)
{
    std::wstring result;
    int start = src.find_first_not_of(L"\r\n\t ");
    if (start == std::wstring::npos)
        start = 0;
    int end = src.find_last_not_of(L"\r\n\t ");
    if (end != std::wstring::npos)
        end += 1;
    return src.substr(start, end);
}

std::vector<DatabaseRow> MakeupDBFromBytes(WCHAR* src)
{
    std::wstringstream srcStream(src);
    std::vector<DatabaseRow> result;
    std::wstring bufLine;
    std::wstringstream bufStream;
    int index = 0;
    while (std::getline(srcStream, bufLine))
    {
        bufStream.clear();
        bufStream << trim(bufLine);
        result.push_back(GetDBRowFromText(bufStream, index++));  //Порядок вычисления индекса?
    }

    //TODO

    return result;
}

void ConcatByteArrays(char* first, int firstSize, char* second, int secSize, char* dest)
{
    std::copy(first, &first[firstSize], dest);
    std::copy(second, &second[secSize], &dest[firstSize]);
}

std::vector<DatabaseRow> LinearSearchBigDB(std::wstring fileName, DatabaseRow dbRow, unsigned int frameSize = 65536)
{
    std::vector<DatabaseRow> result = {};

    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DWORD dw = GetLastError();
        LPTSTR lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);
        return result;
    }

    long long fileSize = GetFileSize(hFile, NULL);
    long long remainSize = fileSize;
    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, remainSize, L"");
    if (hMapping == NULL)
    {
        CloseHandle(hFile);
        return result;
    }

    long long mappingOffset = 0;
    long long validRange;
    WCHAR* convertedWChars = new WCHAR[static_cast<size_t>(frameSize)*2];
    char* remainBytes = new char[static_cast<size_t>(frameSize)];
    char* concatedBytes = new char[static_cast<size_t>(frameSize)*2];
    unsigned int remainBytesCount = 0;
    while (remainSize > 0)  //mappingOffset < fileSize
    {  
        if (remainSize > frameSize)
            validRange = frameSize;
        else
            validRange = remainSize;
        char *mappedBytes = (char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, mappingOffset, validRange);
        if (mappedBytes == NULL)
        {
            /*LPTSTR lpMsgBuf;
            DWORD dw = GetLastError();
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf,
                0, NULL);*/

            break;
        }
        if (remainBytesCount > 0)
        {
            ConcatByteArrays(remainBytes, remainBytesCount, mappedBytes, validRange, concatedBytes);
            validRange += remainBytesCount;
        }
        else
            std::copy(mappedBytes, &mappedBytes[validRange], concatedBytes);

        //validRange
        unsigned int mappedArrSize = validRange;
        if (remainSize > frameSize)
        {
            while (validRange > 0 && concatedBytes[validRange - 1] != '\n')
                validRange--;
            std::copy(&concatedBytes[validRange], &concatedBytes[mappedArrSize], remainBytes);
            remainBytesCount = mappedArrSize - validRange;
        }
        else
            remainBytesCount = 0;
        
        size_t wCharsNum = 0;
        errno_t err = mbstowcs_s(&wCharsNum, convertedWChars, static_cast<size_t>(frameSize)*2, concatedBytes, validRange);
        if (err != 0)
        {
            UnmapViewOfFile(mappedBytes);
            break;
        }
        remainSize -= mappedArrSize;
        if (remainSize > frameSize)
            mappingOffset += frameSize;
        std::vector<DatabaseRow> curFrameVector = MakeupDBFromBytes(convertedWChars);
        std::vector<DatabaseRow> searchResult = LinearSearch(curFrameVector, dbRow);
        result.insert(std::end(result), std::begin(searchResult), std::end(searchResult));

        UnmapViewOfFile(mappedBytes);
    }

    delete[] convertedWChars;
    delete[] remainBytes;
    delete[] concatedBytes;
    CloseHandle(hMapping);
    CloseHandle(hFile);
    return result;
}


