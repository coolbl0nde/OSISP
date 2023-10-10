// lab4.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab4.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hStatusLabel = nullptr;
HWND hCombo1 = nullptr;
HWND hCombo2 = nullptr;
HWND hCombo3 = nullptr;
HWND hResultLabel1 = nullptr;
HWND hResultLabel2 = nullptr;
HWND hResultLabel3 = nullptr;


// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#include <vector>
#include <thread>

std::vector<std::thread> threads;

int result1 = 0;
int result2 = 0;
int result3 = 0;

#include <cmath> // для std::sin

void processData(int threadPriority, int* result)
{
    SetThreadPriority(GetCurrentThread(), threadPriority);

    // Фиктивная обработка данных
    for (int i = 0; i < 100000000; ++i) {
        *result += i;
        volatile double tmp = std::sin(i);
        //Sleep(10);
    }

    if (result == &result1)
        PostMessage(GetParent(hResultLabel1), WM_USER + 1, (WPARAM)&result1, (LPARAM)hResultLabel1);
    else if (result == &result2)
        PostMessage(GetParent(hResultLabel2), WM_USER + 1, (WPARAM)&result2, (LPARAM)hResultLabel2);
    else
        PostMessage(GetParent(hResultLabel3), WM_USER + 1, (WPARAM)&result3, (LPARAM)hResultLabel3);
}

void SetResultLabel(HWND label, int value)
{
    wchar_t resultText[100];
    swprintf_s(resultText, L"Result: %d", value);
    SetWindowText(label, resultText);
}

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
    LoadStringW(hInstance, IDC_LAB4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB4));

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
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB4));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB4);
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
    case WM_CREATE:
    {
        // Создать кнопки "Start" и "Stop"
        CreateWindow(L"BUTTON", L"Start", WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            10, 10, 100, 30, hWnd, (HMENU)IDC_START_BUTTON, hInst, NULL);

        //Создать выпадающий список для установки приоритета потока
        hCombo1 = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
            230, 50, 150, 100, hWnd, (HMENU)IDC_PRIORITY_COMBO1, hInst, NULL);

        hCombo2 = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
            230, 80, 150, 100, hWnd, (HMENU)IDC_PRIORITY_COMBO2, hInst, NULL);

        hCombo3 = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
            230, 110, 150, 100, hWnd, (HMENU)IDC_PRIORITY_COMBO3, hInst, NULL);

        SendMessage(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"Low Priority");
        SendMessage(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"Normal Priority");
        SendMessage(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"High Priority");
        SendMessage(hCombo1, CB_SETCURSEL, 1, 0);

        SendMessage(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"Low Priority");
        SendMessage(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"Normal Priority");
        SendMessage(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"High Priority");
        SendMessage(hCombo2, CB_SETCURSEL, 1, 0); 

        SendMessage(hCombo3, CB_ADDSTRING, 0, (LPARAM)L"Low Priority");
        SendMessage(hCombo3, CB_ADDSTRING, 0, (LPARAM)L"Normal Priority");
        SendMessage(hCombo3, CB_ADDSTRING, 0, (LPARAM)L"High Priority");
        SendMessage(hCombo3, CB_SETCURSEL, 1, 0); 

        hResultLabel1 = CreateWindow(L"STATIC", L"Result 1: 0", WS_VISIBLE | WS_CHILD,
            10, 50, 200, 20, hWnd, (HMENU)IDC_RESULT_LABEL1, hInst, NULL);

        hResultLabel2 = CreateWindow(L"STATIC", L"Result 2: 0", WS_VISIBLE | WS_CHILD,
            10, 80, 200, 20, hWnd, (HMENU)IDC_RESULT_LABEL2, hInst, NULL);

        hResultLabel3 = CreateWindow(L"STATIC", L"Result 3: 0", WS_VISIBLE | WS_CHILD,
            10, 110, 200, 20, hWnd, (HMENU)IDC_RESULT_LABEL3, hInst, NULL);

    }
    break;
    case WM_USER + 1:
    {
        int* pResult = (int*)wParam;
        HWND hResultLabel = (HWND)lParam;

        if (pResult && hResultLabel)
        {
            wchar_t resultText[100];
            swprintf_s(resultText, L"Result: %d", *pResult);
            SetWindowText(hResultLabel, resultText);
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDC_START_BUTTON:
        {
            int selectedPriority1 = SendMessage(hCombo1, CB_GETCURSEL, 0, 0);
            int selectedPriority2 = SendMessage(hCombo2, CB_GETCURSEL, 0, 0);
            int selectedPriority3 = SendMessage(hCombo3, CB_GETCURSEL, 0, 0);

            int threadPriority1 = THREAD_PRIORITY_NORMAL;
            int threadPriority2 = THREAD_PRIORITY_NORMAL;
            int threadPriority3 = THREAD_PRIORITY_NORMAL;

            if (selectedPriority1 == 0) {
                threadPriority1 = THREAD_PRIORITY_LOWEST;
            }
            else if (selectedPriority1 == 2) {
                threadPriority1 = THREAD_PRIORITY_HIGHEST;
            }

            if (selectedPriority2 == 0) {
                threadPriority2 = THREAD_PRIORITY_LOWEST;
            }
            else if (selectedPriority2 == 2) {
                threadPriority2 = THREAD_PRIORITY_HIGHEST;
            }

            if (selectedPriority3 == 0) {
                threadPriority3 = THREAD_PRIORITY_LOWEST;
            }
            else if (selectedPriority3 == 2) {
                threadPriority3 = THREAD_PRIORITY_HIGHEST;
            }

            // Запустить потоки для обработки данных с выбранными приоритетами
            std::thread thread1(processData, threadPriority1, &result1);
            std::thread thread2(processData, threadPriority2, &result2);
            std::thread thread3(processData, threadPriority3, &result3);

            thread1.detach();
            thread2.detach();
            thread3.detach();
        }
        break;
        case IDM_EXIT:
        {
            DestroyWindow(hWnd);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
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
