// Lab1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include <iostream>
#include <string>
//#include "wingdi.h""
#include "Lab1.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    int flag;
    // Цикл основного сообщения:
    while (flag = GetMessage(&msg, nullptr, 0, 0))
    {
        if (flag == -1)
            break;
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
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
const int RB_LINE = 10000;
const int RB_POLYLINE = 10001;
const int RB_RECTANGLE = 10002;
const int RB_POLYGON = 10003;
const int RB_ELIPSE = 10004;
const int RB_TEXT = 10005;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   
   CreateWindowA((LPCSTR)"BUTTON", (LPCSTR)"Line", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTORADIOBUTTON, 10 , 10, 80, 20, hWnd, (HMENU)RB_LINE, hInstance, NULL);
   CreateWindowA((LPCSTR)"BUTTON", (LPCSTR)"Polyline", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 40, 80, 20, hWnd, (HMENU)RB_POLYLINE, hInstance, NULL);
   CreateWindowA((LPCSTR)"BUTTON", (LPCSTR)"Rectangle", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 70, 80, 20, hWnd, (HMENU)RB_RECTANGLE, hInstance, NULL);
   CreateWindowA((LPCSTR)"BUTTON", (LPCSTR)"Polygon", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 100, 80, 20, hWnd, (HMENU)RB_POLYGON, hInstance, NULL);
   CreateWindowA((LPCSTR)"BUTTON", (LPCSTR)"Elipse", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 130, 80, 20, hWnd, (HMENU)RB_ELIPSE, hInstance, NULL);
   CreateWindowA((LPCSTR)"BUTTON", (LPCSTR)"Text", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 160, 80, 20, hWnd, (HMENU)RB_TEXT, hInstance, NULL);

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

BOOL AddToPointsArr(POINT** arr, POINT elem, int prevElemNum)  //Not in use
{
    POINT* newArr;
    if (*arr == NULL)
        newArr = (POINT*)malloc(sizeof(POINT));
    else
        newArr = (POINT*)realloc((POINT*)(*arr), (prevElemNum + 1) * sizeof(POINT));
    if (newArr == NULL)    
        return 0;
    *arr = newArr;
    (*arr)[prevElemNum] = elem;
    return 1;
}

void ClearPointsArr(POINT** arr)  
{
    free(*arr);
    *arr = NULL;
}

POINTS prevMousePos;
POINT polygonFirstPoint;
bool isNewPolyline = true;
bool isNewPolygon = true;
POINT* polygonPoints;
int polygonPointsCount = 0;
int curCheckedRB;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        HDC hdc = GetDC(hWnd);
        POINTS  curMousePos = MAKEPOINTS(lParam);  

        switch (curCheckedRB)
        {
        case RB_LINE:
            prevMousePos = curMousePos;
            break;
        case RB_POLYLINE:
            if (isNewPolyline && (wParam & MK_SHIFT))
            {
                prevMousePos = curMousePos;
                isNewPolyline = false;
            }
            else if (!isNewPolyline && (wParam & MK_SHIFT))
            {
                MoveToEx(hdc, prevMousePos.x, prevMousePos.y, NULL);
                LineTo(hdc, curMousePos.x, curMousePos.y);
                prevMousePos = curMousePos;
            }
            break;
        case RB_RECTANGLE: 
            prevMousePos = curMousePos;
            break;
        case RB_POLYGON:  
            POINT bufPoint;
            bufPoint.x = curMousePos.x;
            bufPoint.y = curMousePos.y;
            if (isNewPolygon && (wParam & MK_SHIFT))
            {               
                AddToPointsArr(&polygonPoints, bufPoint, 0);
                polygonPointsCount++;
                isNewPolygon = false;
            }
            else if (!isNewPolygon && (wParam & MK_SHIFT))
            {
                AddToPointsArr(&polygonPoints, bufPoint, polygonPointsCount);
                polygonPointsCount++;
            }
            break;
        case RB_ELIPSE:
            prevMousePos = curMousePos;
            break;
        case RB_TEXT:
            prevMousePos = curMousePos;
            break;
        }          

        ReleaseDC(hWnd, hdc);
        break;
    }
    case WM_LBUTTONUP:
    {
        HDC hdc = GetDC(hWnd);
        POINTS  curMousePos = MAKEPOINTS(lParam);
        switch (curCheckedRB)
        {
        case RB_LINE:
            MoveToEx(hdc, prevMousePos.x, prevMousePos.y, NULL);
            LineTo(hdc, curMousePos.x, curMousePos.y);
            break;
        case RB_RECTANGLE:
            Rectangle(hdc, prevMousePos.x, prevMousePos.y, curMousePos.x, curMousePos.y);
            break;
        case RB_ELIPSE:
            Ellipse(hdc, prevMousePos.x, prevMousePos.y, curMousePos.x, curMousePos.y);
            break;
        }
        ReleaseDC(hWnd, hdc);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case RB_LINE:
            curCheckedRB = RB_LINE;
            break;
        case RB_POLYLINE:
            curCheckedRB = RB_POLYLINE;
            break;
        case RB_RECTANGLE:
            curCheckedRB = RB_RECTANGLE;
            break;
        case RB_POLYGON:
            curCheckedRB = RB_POLYGON;
            break;
        case RB_ELIPSE:
            curCheckedRB = RB_ELIPSE;
            break;
        case RB_TEXT:
            curCheckedRB = RB_TEXT;
            prevMousePos.x = 0;
            prevMousePos.y = 0;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        SetFocus(hWnd);
        break;
    }
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_SHIFT:
            if ((lParam & 0x40000000) == 0) //30ый бит lParam, флаг, была ли клавиша уже нажата
            {
                isNewPolyline = true;
                isNewPolygon = true;
                polygonPointsCount = 0;
            }
            break;
        }
        break;
    }
    case WM_KEYUP:    
        if (wParam == VK_SHIFT && curCheckedRB == RB_POLYGON)
        {
            HDC hdc = GetDC(hWnd);
            SelectObject(hdc, GetStockObject(DC_BRUSH));
            SetDCBrushColor(hdc, RGB(0, 255, 0));
            Polygon(hdc, polygonPoints, polygonPointsCount);
            ClearPointsArr(&polygonPoints);
            ReleaseDC(hWnd, hdc);
        }
        break;
    case WM_CHAR:
        switch (wParam)
        {
        case 0x08:  //Backspace
            break;
        case 0x0A:  //Linefeed
            break;
        case 0x1B:  //Escape
            break;
        case 0x09:  //Tab
            break;
        case 0x0D:  //Carriage return
            break;
        default:
            if (curCheckedRB == RB_TEXT)
            {
                int caretX = prevMousePos.x;
                int caretY = prevMousePos.y;
                char curChar = (char)wParam;
                int charWidth;

                HDC hdc = GetDC(hWnd);
                GetCharWidth32A(hdc, (UINT)curChar, (UINT)curChar, &charWidth);
                TextOutA(hdc, caretX, caretY, &curChar, 1);
                ReleaseDC(hWnd, hdc);

                prevMousePos.x += charWidth;
                break;
            }
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