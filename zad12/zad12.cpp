// zad12.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "zad12.h"
#include <windowsx.h>
#include<Windows.h>
#include <Psapi.h>
#include <Strsafe.h>
#pragma comment(lib, "Psapi.lib")

#define MAX_LOADSTRING 100


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ZAD12, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZAD12));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZAD12));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ZAD12);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
void LoadProcessesToListBox(HWND hwndCtl)
{
    ListBox_ResetContent(hwndCtl);

    DWORD aProcessIds[1024], cbNeeded = 0;
    BOOL bRet = EnumProcesses(aProcessIds, sizeof(aProcessIds), &cbNeeded);
    if (FALSE != bRet)
    {
        TCHAR szName[MAX_PATH], szBuffer[300];

        for (DWORD i = 0, n = cbNeeded / sizeof(DWORD); i < n; ++i)
        {
            DWORD dwProcessId = aProcessIds[i], cch = 0;
            if (0 == dwProcessId) continue;

            HANDLE hPocess = OpenProcess(PROCESS_VM_READ |
                PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

            if (NULL != hPocess)
            {
                cch = GetModuleBaseName(hPocess, NULL, szName, MAX_PATH);
                CloseHandle(hPocess);
            }
            if (0 == cch)
                StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));

            StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), szName, dwProcessId);

            int iItem = ListBox_AddString(hwndCtl, szBuffer);
            ListBox_SetItemData(hwndCtl, iItem, dwProcessId);
        }
    }

}

void LoadModulesToListBox(HWND hwndCtl,DWORD dwProcessId)
{
    ListBox_ResetContent(hwndCtl);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ, FALSE, dwProcessId);

    if (NULL != hProcess)
    {
        DWORD cb = 0;
        EnumProcessModulesEx(hProcess, NULL, 0, &cb, LIST_MODULES_ALL);

        DWORD nCount = cb / sizeof(HMODULE);

        HMODULE* hModule = new HMODULE[nCount];

        cb = nCount * sizeof(HMODULE);
        BOOL bRet = EnumProcessModulesEx(hProcess, hModule, cb, &cb, LIST_MODULES_ALL);

        if (FALSE != bRet)
        {
            TCHAR szFileName[MAX_PATH];

            for (DWORD i = 0; i < nCount; ++i)
            {
                bRet = GetModuleFileNameEx(hProcess, hModule[i], szFileName, MAX_PATH);
                if (FALSE != bRet) ListBox_AddString(hwndCtl, szFileName);
            }
        }
        delete[]hModule;
        CloseHandle(hProcess);
    }


   

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
   HWND hListProcesses = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD,
       50, 50, 200, 200, hWnd, NULL, hInstance, NULL);

   HWND hListmoduls = CreateWindowEx(0, TEXT("ListBox"), NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD,
       400, 50, 200, 200, hWnd, NULL, hInstance, NULL);

   LoadProcessesToListBox(hListProcesses);
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


