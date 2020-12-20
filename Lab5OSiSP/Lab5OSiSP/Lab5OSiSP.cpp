#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <list>

#include "framework.h"
#include "Lab5OSiSP.h"
#include "DatabaseLibrary.h"

#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100

const wchar_t* headers[] = {
        { L"Telephone" },
        { L"First name" },
        { L"Second name" },
        { L"Patronymic" },
        { L"Street" },
        { L"House" },
        { L"Building" },
        { L"Apartment" }
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
const WCHAR* szTitle = L"Lab4";
const WCHAR* szWindowClass = L"MainWindowClass";
std::wstring databaseName;

#define IDC_LISTVIEW (HMENU)1001
#define IDC_PHONEEDIT (HMENU)1002
#define IDC_FNEDIT (HMENU)1003
#define IDC_SNEDIT (HMENU)1004
#define IDC_PATRONYMICEDIT (HMENU)1005
#define IDC_HSEDIT (HMENU)1006
#define IDC_BLDNGEDIT (HMENU)1007
#define IDC_APARTMENTEDIT (HMENU)1008
#define IDC_STREETEDIT (HMENU)1009
#define IDC_SEARCHBTN (HMENU)1010
#define IDC_REFRESHBTN (HMENU)1011

typedef struct TMainWindow {
    HWND hWnd;
    HWND hListView;
    HWND hPhoneNum;
    HWND hFirstName;
    HWND hSecondName;
    HWND hPatronymic;
    HWND hHouseNum;
    HWND hBuildingNum;
    HWND hApartmentNum;
    HWND hStreet;
    HWND hSearch;
    HWND hRefresh;
}TMainWindow, * PMainWindow;

std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

HWND CreateListView(HWND parent, int columns) {

    RECT rcClient;
    GetClientRect(parent, &rcClient);

    HWND hWndListView = CreateWindow(WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_EDITLABELS | WS_EX_CLIENTEDGE, 0, rcClient.top + 50,
        rcClient.right - rcClient.left, rcClient.bottom - rcClient.top - 50, parent, IDC_LISTVIEW, hInst, NULL);

    LVCOLUMN c = { 0 };
    c.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int i = 0; i < columns; i++) {
        c.fmt = LVCFMT_LEFT;
        c.pszText = (LPWSTR)headers[i];
        c.iSubItem = i;
        c.cx = 100;
        ListView_InsertColumn(hWndListView, i, &c);
    }
    return hWndListView;
}

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

extern std::vector<DatabaseRow> dbRowsList;
extern std::vector<ByStringIndex> secNameIndex, phoneNumIndex, streetIndex;

typedef HRESULT(*LoadDBFromFileFunc)(const std::wstring, std::vector<DatabaseRow>&);
typedef HRESULT(*MakeupIndexByStringFunc)(std::vector<DatabaseRow>&, std::vector<ByStringIndex>&, int);
typedef std::vector<DatabaseRow>(*BinarySearchFunc)(DatabaseRow);
typedef std::vector<DatabaseRow>(*LinearSearchFunc)(std::vector<DatabaseRow>, DatabaseRow);
typedef std::vector<DatabaseRow>(*LinearSearchBigDBFunc)(std::wstring, DatabaseRow, unsigned int);

LoadDBFromFileFunc loadDBFunc;
MakeupIndexByStringFunc indByStringFunc;
BinarySearchFunc binSearchFunc;
LinearSearchFunc linearSearchFunc;
LinearSearchBigDBFunc linearSearchBigDBFunc;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    HINSTANCE myDll = ::LoadLibrary(L"DatabaseLibrary.dll");
    if (myDll < (HINSTANCE)HINSTANCE_ERROR)
    {        
        return -1;
    }
    loadDBFunc = (LoadDBFromFileFunc)::GetProcAddress(myDll, "LoadDatabaseFromFile");
    indByStringFunc = (MakeupIndexByStringFunc)::GetProcAddress(myDll, "MakeupIndexByString");
    binSearchFunc = (BinarySearchFunc)::GetProcAddress(myDll, "BinarySearch");
    linearSearchFunc = (LinearSearchFunc)::GetProcAddress(myDll, "LinearSearch");
    linearSearchBigDBFunc = (LinearSearchBigDBFunc)::GetProcAddress(myDll, "LinearSearchBigDB");

    if (loadDBFunc == NULL || indByStringFunc == NULL 
        || binSearchFunc == NULL || linearSearchFunc == NULL
        || linearSearchBigDBFunc == NULL)
    {
        FreeLibrary(myDll);
        return -1;
    }

    databaseName = ExePath() + L"\\DatabaseLarge.txt";
    /*SYSTEM_INFO si;
    GetSystemInfo(&si);*/


    hInst = hInstance;

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof icc;
    icc.dwICC = ICC_COOL_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);

    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5OSISP));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

void UpdateList(HWND hListView, std::vector<DatabaseRow> records) {
    ListView_DeleteAllItems(hListView);
    LVITEM item;
    ZeroMemory(&item, sizeof(LVITEM));
    item.mask = LVIF_TEXT | LVIF_STATE;
    for (auto it = records.begin(); it != records.end(); it++) {
        auto i = std::distance(records.begin(), it);
        item.iItem = i;
        item.iSubItem = 0;
        item.pszText = (LPWSTR)(*it).phoneNum.c_str();
        ListView_InsertItem(hListView, &item);
        ListView_SetItemText(hListView, i, 1, (LPWSTR)(*it).firstName.c_str());
        ListView_SetItemText(hListView, i, 2, (LPWSTR)(*it).secondName.c_str());
        ListView_SetItemText(hListView, i, 3, (LPWSTR)(*it).patronymic.c_str());
        ListView_SetItemText(hListView, i, 4, (LPWSTR)(*it).street.c_str());
        std::wstring tmp = std::to_wstring((*it).houseNum);
        ListView_SetItemText(hListView, i, 5, (LPWSTR)tmp.c_str());
        tmp = std::to_wstring((*it).buildingNum);
        ListView_SetItemText(hListView, i, 6, (LPWSTR)tmp.c_str());
        tmp = std::to_wstring((*it).apartmentNum);
        ListView_SetItemText(hListView, i, 7, (LPWSTR)tmp.c_str());
    }
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 4;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB5OSISP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB5OSISP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

PMainWindow pSelf;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE) {
        pSelf = (PMainWindow)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TMainWindow));
        if (pSelf != NULL) {
            pSelf->hWnd = hWnd;
            pSelf->hListView = CreateListView(hWnd, 8);
            pSelf->hPhoneNum = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                3, 3, 95, 20, hWnd, IDC_PHONEEDIT, hInst, NULL);
            pSelf->hFirstName = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                103, 3, 95, 20, hWnd, IDC_FNEDIT, hInst, NULL);
            pSelf->hSecondName = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                203, 3, 95, 20, hWnd, IDC_SNEDIT, hInst, NULL);
            pSelf->hPatronymic = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                303, 3, 95, 20, hWnd, IDC_PATRONYMICEDIT, hInst, NULL);
            pSelf->hStreet = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                403, 3, 95, 20, hWnd, IDC_STREETEDIT, hInst, NULL);
            pSelf->hHouseNum = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_NUMBER,
                503, 3, 95, 20, hWnd, IDC_HSEDIT, hInst, NULL);
            pSelf->hBuildingNum = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_NUMBER,
                603, 3, 95, 20, hWnd, IDC_BLDNGEDIT, hInst, NULL);
            pSelf->hApartmentNum = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_NUMBER,
                703, 3, 75, 20, hWnd, IDC_APARTMENTEDIT, hInst, NULL);
            pSelf->hSearch = CreateWindow(WC_BUTTON, TEXT("Search"), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | BS_CENTER,
                3, 25, 700, 23, hWnd, IDC_SEARCHBTN, hInst, NULL);
            pSelf->hRefresh = CreateWindow(WC_BUTTON, TEXT("Refresh"), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | BS_CENTER,
                703, 25, 74, 23, hWnd, IDC_REFRESHBTN, hInst, NULL);

            SetWindowText(pSelf->hHouseNum, L"0");
            SetWindowText(pSelf->hBuildingNum, L"0");
            SetWindowText(pSelf->hApartmentNum, L"0");


            //UpdateList(pSelf->hListView, dbRowsList);


            SetWindowLongPtr(hWnd, 0, (LONG)pSelf);

        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    //PAINTSTRUCT ps;
    //HDC hdc;

    //pSelf = (PMainWindow)GetWindowLong(hWnd, 0);
    if (!pSelf) {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    switch (message) {
    case WM_COMMAND:
        if (LOWORD(wParam) == (WORD)IDC_SEARCHBTN) {
            wchar_t* phoneNumBuff = new wchar_t[1024];
            wchar_t* firstNameBuff = new wchar_t[1024];
            wchar_t* patronymicBuff = new wchar_t[1024];
            wchar_t* secNameBuff = new wchar_t[1024];
            wchar_t* streetBuff = new wchar_t[1024];
            wchar_t* houseNumBuff = new wchar_t[1024];
            wchar_t* buildingNumBuff = new wchar_t[1024];
            wchar_t* apartmentNumBuff = new wchar_t[1024];

            GetWindowText(pSelf->hPhoneNum, (LPWSTR)phoneNumBuff, 1024);
            GetWindowText(pSelf->hFirstName, (LPWSTR)firstNameBuff, 1024);
            GetWindowText(pSelf->hSecondName, (LPWSTR)secNameBuff, 1024);
            GetWindowText(pSelf->hPatronymic, (LPWSTR)patronymicBuff, 1024);
            GetWindowText(pSelf->hStreet, (LPWSTR)streetBuff, 1024);
            GetWindowText(pSelf->hHouseNum, (LPWSTR)houseNumBuff, 1024);
            GetWindowText(pSelf->hBuildingNum, (LPWSTR)buildingNumBuff, 1024);
            GetWindowText(pSelf->hApartmentNum, (LPWSTR)apartmentNumBuff, 1024);

            std::wstring phoneNum(phoneNumBuff);
            std::wstring firstName(firstNameBuff);
            std::wstring patronymic(patronymicBuff);
            std::wstring secName(secNameBuff);
            std::wstring street(streetBuff);
            std::wstring houseNum(houseNumBuff);
            std::wstring buildingNum(buildingNumBuff);
            std::wstring apartmentNum(apartmentNumBuff);

            DatabaseRow dbr;
            dbr.ind = 0;
            dbr.phoneNum = phoneNum;
            dbr.firstName = firstName;
            dbr.secondName = secName;
            dbr.patronymic = patronymic;
            dbr.street = street;
            dbr.houseNum = std::stoi(houseNum);
            if (dbr.houseNum == 0)
                dbr.houseNum = -1;
            dbr.buildingNum = std::stoi(buildingNum);
            if (dbr.buildingNum == 0)
                dbr.buildingNum = -1;
            dbr.apartmentNum = std::stoi(apartmentNum);
            if (dbr.apartmentNum == 0)
                dbr.apartmentNum = -1;
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            UpdateList(pSelf->hListView, linearSearchBigDBFunc(databaseName, dbr, si.dwAllocationGranularity));
        }
        else if (LOWORD(wParam) == (WORD)IDC_REFRESHBTN) {
            UpdateList(pSelf->hListView, dbRowsList);
        }
        break;
    case WM_DESTROY:
        //phoneBook.clear();
        HeapFree(GetProcessHeap(), 0, pSelf);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
