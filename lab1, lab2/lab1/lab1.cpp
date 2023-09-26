// lab1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "helpers.h"
#include "lab1.h"

#include <commdlg.h>
#include <fstream>
#include <richedit.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hEdit;                                     // 

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
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    LoadLibrary(TEXT("riched20.dll"));
    LoadLibrary(TEXT("Msftedit.dll"));
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

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
    static WCHAR fileName[MAX_PATH] = L"";
    static COLORREF rgbCurrentText = RGB(0, 0, 0);
    static COLORREF rgbCurrentBackground = RGB(255, 255, 255);

    switch (message)
    {
    case WM_CREATE: // Сообщение о создании окна
        {
            CreateWindow(L"BUTTON", L"Копировать", WS_CHILD | WS_VISIBLE, 10, 10, 100, 30, hWnd, (HMENU)COPY_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Вставить", WS_CHILD | WS_VISIBLE, 120, 10, 100, 30, hWnd, (HMENU)PASTE_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Вырезать", WS_CHILD | WS_VISIBLE, 230, 10, 100, 30, hWnd, (HMENU)CUT_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Очистить", WS_CHILD | WS_VISIBLE, 340, 10, 100, 30, hWnd, (HMENU)CLEAN_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Цвет текста", WS_CHILD | WS_VISIBLE, 500, 10, 100, 30, hWnd, (HMENU)CHANGE_TEXT_COLOR_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Цвет фона", WS_CHILD | WS_VISIBLE, 610, 10, 100, 30, hWnd, (HMENU)CHANGE_BACKGROUND_COLOR_BUTTON, hInst, NULL);
            
            CreateWindowEx(
                WS_EX_CLIENTEDGE, 
                RICHEDIT_CLASS,  
                L"",
                WS_CHILD | WS_VISIBLE, 
                720, 
                10, 
                150, 
                30, 
                hWnd, 
                (HMENU)FONT_SIZE_EDIT,
                hInst, 
                NULL
            );
            
            hEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                RICHEDIT_CLASS,
                L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
                10,
                100,
                500,
                400,
                hWnd,
                NULL,
                hInst,
                NULL
            );
            CreateWindow(L"BUTTON", L"Изменить размер", WS_CHILD | WS_VISIBLE, 720, 50, 150, 30, hWnd, (HMENU)CHANGE_FONT_SIZE_BUTTON, hInst, NULL);

            CreateWindow(L"BUTTON", L"Нормальный", WS_CHILD | WS_VISIBLE, 880, 10, 100, 25, hWnd, (HMENU)NORMAL_TEXT_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Подчеркнутый", WS_CHILD | WS_VISIBLE, 880, 40, 100, 25, hWnd, (HMENU)UNDERLINE_TEXT_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Наклонный", WS_CHILD | WS_VISIBLE, 880, 70, 100, 25, hWnd, (HMENU)ITALIC_TEXT_BUTTON, hInst, NULL);
            CreateWindow(L"BUTTON", L"Жирный", WS_CHILD | WS_VISIBLE, 1000, 70, 70, 25, hWnd, (HMENU)BOLD_TEXT_BUTTON, hInst, NULL);
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
                {
                case EXIT:
                    DestroyWindow(hWnd);
                    break;

                case OPEN_BUTTON:
                {
                    std::wstring openedFileName = HandleOpen(hWnd, hEdit);
                    wcscpy_s(fileName, openedFileName.c_str());
                    break;
                }

                case SAVE_BUTTON:
                {
                    if (wcslen(fileName) > 0)
                    {
                        WCHAR szNewContent[10000];
                        GetWindowText(hEdit, szNewContent, 10000);
                        std::wstring FilteredContent = FilterContent(szNewContent);
                        SaveToFile(hWnd, fileName, FilteredContent.c_str());
                    }
                    else
                    {
                        HandleSaveAs(hWnd, hEdit);
                    }
                }
                break;

                case SAVE_AS_BUTTON:
                    HandleSaveAs(hWnd, hEdit);
                    break;

                case COPY_BUTTON:
                    SendMessage(hEdit, WM_COPY, 0, 0);
                    break;
                case PASTE_BUTTON:
                    SendMessage(hEdit, WM_PASTE, 0, 0);
                    break;
                case CUT_BUTTON:
                    SendMessage(hEdit, WM_CUT, 0, 0);
                    break;
                case CLEAN_BUTTON:
                    SetWindowText(hEdit, L"");
                    break;

                case CHANGE_TEXT_COLOR_BUTTON:
                    {
                        CHARFORMAT2 cf;
                        ZeroMemory(&cf, sizeof(cf));
                        cf.cbSize = sizeof(cf);
                        cf.dwMask = CFM_COLOR | CFM_BACKCOLOR;

                        SendMessage(hEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

                        DWORD currentBackgroundColor = cf.crBackColor;

                        CHOOSECOLOR cc;
                        static COLORREF acrCustClr[16];
 
                        ZeroMemory(&cc, sizeof(cc));
                        cc.lStructSize = sizeof(cc);
                        cc.hwndOwner = hWnd;
                        cc.lpCustColors = (LPDWORD)acrCustClr;
                        cc.rgbResult = rgbCurrentText;
                        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

                        if (ChooseColor(&cc) == TRUE)
                        {
                            rgbCurrentText = cc.rgbResult;

                            CHARRANGE selRange;
                            SendMessage(hEdit, EM_EXGETSEL, 0, (LPARAM)&selRange);

                            if (selRange.cpMin != selRange.cpMax)
                            {
                                CHARFORMAT2 cf;
                                ZeroMemory(&cf, sizeof(cf));
                                cf.cbSize = sizeof(cf);
                                cf.dwMask = CFM_COLOR;
                                cf.crBackColor = currentBackgroundColor;
                                cf.crTextColor = rgbCurrentText;

                                SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                            }
                        }
                    }
                    break;

                case CHANGE_BACKGROUND_COLOR_BUTTON:
                    {
                        CHARFORMAT2 cf;
                        ZeroMemory(&cf, sizeof(cf));
                        cf.cbSize = sizeof(cf);
                        cf.dwMask = CFM_COLOR | CFM_BACKCOLOR;

                        SendMessage(hEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

                        DWORD currentTextColor = cf.crTextColor;

                        // Prepare and open the CHOOSECOLOR dialog for background color
                        CHOOSECOLOR cc;
                        static COLORREF acrCustClr[16]; // Array of custom colors 
                        DWORD rgbCurrent;

                        ZeroMemory(&cc, sizeof(cc));
                        cc.lStructSize = sizeof(cc);
                        cc.hwndOwner = hWnd;
                        cc.lpCustColors = (LPDWORD)acrCustClr;
                        cc.rgbResult = cf.crBackColor; // use current background color
                        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

                        if (ChooseColor(&cc) == TRUE)
                        {
                            rgbCurrent = cc.rgbResult; // Get the selected background color

                            cf.dwMask = CFM_COLOR | CFM_BACKCOLOR;
                            cf.dwEffects &= ~CFE_AUTOBACKCOLOR; // Clear the auto background color flag
                            cf.crTextColor = currentTextColor; // Preserve the original text color
                            cf.crBackColor = rgbCurrent; // Update background color

                            SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                        }
                    }
                    break;

                case CHANGE_FONT_SIZE_BUTTON:
                    {
                        // Retrieve the font size from the Edit Control
                        wchar_t buf[100];
                        GetDlgItemTextW(hWnd, FONT_SIZE_EDIT, buf, sizeof(buf));
                        int selectedSize = _wtoi(buf);

                        // Check if the input is numeric and in a valid range
                        if (selectedSize <= 0 || !isdigit(buf[0]) || selectedSize < 5 || selectedSize > 72)
                        {
                            MessageBox(hWnd, L"Введите число между 5 и 72!", L"Ошибка", MB_OK);
                            return 0;
                        }

                        // Convert size to twips
                        LONG newSizeInTwips = selectedSize * 20;

                        // Prepare CHARFORMAT2 structure
                        CHARFORMAT2 cf;
                        ZeroMemory(&cf, sizeof(cf));
                        cf.cbSize = sizeof(cf);
                        cf.dwMask = CFM_SIZE;
                        cf.yHeight = newSizeInTwips;

                        SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                    }
                    break;

                case NORMAL_TEXT_BUTTON:
                {
                    CHARFORMAT2 cf;
                    ZeroMemory(&cf, sizeof(cf));
                    cf.cbSize = sizeof(cf);
                    cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
                    cf.dwEffects = 0;

                    SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                }
                break;

                case BOLD_TEXT_BUTTON:
                {
                    CHARFORMAT2 cf;
                    ZeroMemory(&cf, sizeof(cf));
                    cf.cbSize = sizeof(cf);
                    cf.dwMask = CFM_BOLD;
                    cf.dwEffects = CFE_BOLD;

                    SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                }
                break;

                case ITALIC_TEXT_BUTTON:
                {
                    CHARFORMAT2 cf;
                    ZeroMemory(&cf, sizeof(cf));
                    cf.cbSize = sizeof(cf);
                    cf.dwMask = CFM_ITALIC;
                    cf.dwEffects = CFE_ITALIC;

                    SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                }
                break;

                case UNDERLINE_TEXT_BUTTON:
                {
                    CHARFORMAT2 cf;
                    ZeroMemory(&cf, sizeof(cf));
                    cf.cbSize = sizeof(cf);
                    cf.dwMask = CFM_UNDERLINE;
                    cf.dwEffects = CFE_UNDERLINE;

                    SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
                }
                break;
                       
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }   
        }
        break;

    case WM_KEYDOWN:
    {
        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            switch (wParam)
            {
            case 'S': // Ctrl + S
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(SAVE_BUTTON, 0), 0);
                return 0;
            case 'C': // Ctrl + C
                SendMessage(hEdit, WM_COPY, 0, 0);
                return 0;
            case 'V': // Ctrl + V
                SendMessage(hEdit, WM_PASTE, 0, 0);
                return 0;
            case 'X': // Ctrl + X
                SendMessage(hEdit, WM_CUT, 0, 0);
                return 0;
            }
        }
    }
    break;

    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        int windowWidth = rc.right - rc.left;
        int windowHeight = rc.bottom - rc.top;

        int editHeight = windowHeight - 120;

        SetWindowPos(
            hEdit,
            NULL,
            10,
            100,
            windowWidth - 20,
            editHeight,
            SWP_NOZORDER
        );
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
