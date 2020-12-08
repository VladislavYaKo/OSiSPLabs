// Lab4OSiSP.cpp : Определяет точку входа для приложения.
//
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <list>

#include "framework.h"
#include "Lab4OSiSP.h"
#include "../DatabaseLibrary/DatabaseLibrary.h"

//#include "DatabaseLibrary.h"

#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100

const wchar_t* headers[] = {
        { L"Telephone" },
        { L"First name" },
        { L"Last name" },
        { L"Middle name" },
        { L"Street" },
        { L"House" },
        { L"Housing" },
        { L"Flat" }
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
const WCHAR* szTitle = L"Lab4";
const WCHAR* szWindowClass = L"MainWindowClass";

#define IDC_LISTVIEW (HMENU)101
#define IDC_TELEDIT (HMENU)102
#define IDC_FNEDIT (HMENU)103
#define IDC_MNEDIT (HMENU)104
#define IDC_LNEDIT (HMENU)105
#define IDC_HSEDIT (HMENU)106
#define IDC_HSNGEDIT (HMENU)107
#define IDC_FLTEDIT (HMENU)108
#define IDC_STREDIT (HMENU)109
#define IDC_SEARCHBTN (HMENU)110

typedef struct TMainWindow {
    HWND hWnd;
    HWND hListView;
    HWND hTel;
    HWND hFn;
    HWND hMn;
    HWND hLn;
    HWND hHs;
    HWND hHsng;
    HWND hFlt;
    HWND hStr;
    HWND hSearch;

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
typedef HRESULT(*MakeupIndexByStringFunc)(std::vector<DatabaseRow>&, std::vector<ByStringIndex>&, int indexingFlag);
typedef int(*BinarysearchFunc)(std::vector<ByStringIndex>, int, int, std::wstring);

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
    LoadDBFromFileFunc loadDBFunc = (LoadDBFromFileFunc)::GetProcAddress(myDll, "LoadDatabaseFromFile");
    MakeupIndexByStringFunc indByStringFunc = (MakeupIndexByStringFunc)::GetProcAddress(myDll, "MakeupIndexByString");
    BinarysearchFunc binSearchFunc = (BinarysearchFunc)::GetProcAddress(myDll, "BinarySearch");

    if (loadDBFunc == NULL || indByStringFunc == NULL)
    {
        FreeLibrary(myDll);
        return -1;
    }
    loadDBFunc(ExePath() + std::wstring(L"\\Database.txt"), dbRowsList);

    indByStringFunc(dbRowsList, secNameIndex, INDEXING_BY_SECOND_NAME);
    indByStringFunc(dbRowsList, phoneNumIndex, INDEXING_BY_PHONE_NUMBER);
    indByStringFunc(dbRowsList, streetIndex, INDEXING_BY_STREET);




    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    hInst = hInstance;

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof icc;
    icc.dwICC = ICC_COOL_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);

    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_LAB4OSISP, szWindowClass, MAX_LOADSTRING);
    

    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB4OSISP));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 4;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB4OSISP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB4OSISP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMainWindow pSelf;
    if (message == WM_CREATE) {
        pSelf = (PMainWindow)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TMainWindow));
        if (pSelf != NULL) {
            pSelf->hWnd = hWnd;
            pSelf->hListView = CreateListView(hWnd, 8);
            pSelf->hTel = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                3, 3, 95, 20, hWnd, IDC_TELEDIT, hInst, NULL);
            pSelf->hFn = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                103, 3, 95, 20, hWnd, IDC_FNEDIT, hInst, NULL);
            pSelf->hMn = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                203, 3, 95, 20, hWnd, IDC_MNEDIT, hInst, NULL);
            pSelf->hLn = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                303, 3, 95, 20, hWnd, IDC_LNEDIT, hInst, NULL);
            pSelf->hStr = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                403, 3, 95, 20, hWnd, IDC_FLTEDIT, hInst, NULL);
            pSelf->hHs = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_NUMBER,
                503, 3, 95, 20, hWnd, IDC_HSEDIT, hInst, NULL);
            pSelf->hHsng = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_NUMBER,
                603, 3, 95, 20, hWnd, IDC_HSNGEDIT, hInst, NULL);
            pSelf->hFlt = CreateWindow(WC_EDIT, TEXT(""), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_NUMBER,
                703, 3, 75, 20, hWnd, IDC_FLTEDIT, hInst, NULL);
            pSelf->hSearch = CreateWindow(WC_BUTTON, TEXT("Search"), WS_CHILD | WS_OVERLAPPED | WS_EX_CLIENTEDGE | WS_VISIBLE | WS_BORDER | BS_CENTER,
                3, 25, 774, 23, hWnd, IDC_SEARCHBTN, hInst, NULL);

            SetWindowText(pSelf->hHs, L"0");
            SetWindowText(pSelf->hHsng, L"0");
            SetWindowText(pSelf->hFlt, L"0");

            
            UpdateList(pSelf->hListView, dbRowsList);


            SetWindowLongPtr(hWnd, 0, (LONG)pSelf);

        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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
