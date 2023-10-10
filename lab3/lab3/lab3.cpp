#include "framework.h"
#include "lab3.h"

#include <commdlg.h>
#include <shlobj.h>
#include <string>
#include <thread>
#include <fstream>

HWND hwndButtonPause, hwndProgress, hwndStatusLabel;
std::wstring sourceFilePath, destFolderPath;
bool paused = false, copying = false;

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void UpdateProgress(float percentage, const std::wstring& status) {
    SendMessage(hwndProgress, PBM_SETPOS, (int)(percentage * 100), 0);
    SetWindowText(hwndStatusLabel, status.c_str());
}

void CopyFileAsync() {
    std::ifstream source(sourceFilePath, std::ios::binary);
    std::ofstream dest(destFolderPath + L"\\" + sourceFilePath.substr(sourceFilePath.find_last_of(L"\\") + 1), std::ios::binary);

    if (!source.is_open() || !dest.is_open()) {
        MessageBox(NULL, L"Error opening files!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    source.seekg(0, std::ios::end);
    size_t fileSize = source.tellg();
    source.seekg(0, std::ios::beg);

    const size_t bufferSize = 4096;
    char buffer[bufferSize];
    size_t copiedBytes = 0;

    while (source.read(buffer, bufferSize) && !source.eof()) {
        while (paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        dest.write(buffer, source.gcount());
        copiedBytes += source.gcount();

        float progress = static_cast<float>(copiedBytes) / fileSize;
        std::wstring status = std::to_wstring((int)(progress * 100)) + L"% (" + std::to_wstring(copiedBytes / (1024 * 1024)) + L" MB)";
        UpdateProgress(progress, status);

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Simulate slower copying for demonstration
    }

    source.close();
    dest.close();

    copying = false;
    UpdateProgress(0.0f, L"");
    MessageBox(NULL, L"Copy completed!", L"Success", MB_OK);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB3));

    MSG msg;
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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        hwndProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, 170, 50, 200, 20, hWnd, NULL, hInst, NULL);
        hwndStatusLabel = CreateWindow(L"STATIC", L"", WS_CHILD | WS_VISIBLE, 170, 80, 200, 20, hWnd, NULL, hInst, NULL);

        CreateWindow(L"BUTTON", L"Выбрать файл", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 100, 30, hWnd, (HMENU)IDC_BUTTONSOURCE, hInst, NULL);
        CreateWindow(L"BUTTON", L"Выбрать директорию", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 120, 10, 150, 30, hWnd, (HMENU)IDC_BUTTONDEST, hInst, NULL);
        CreateWindow(L"BUTTON", L"Старт", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 280, 10, 100, 30, hWnd, (HMENU)IDC_BUTTONSTART, hInst, NULL);
        hwndButtonPause = CreateWindow(L"BUTTON", L"Пауза", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 390, 10, 100, 30, hWnd, (HMENU)IDC_BUTTONPAUSE, hInst, NULL);
        break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId)
        {
        case IDC_BUTTONSOURCE:
        {
            OPENFILENAME ofn = {};
            wchar_t fileName[MAX_PATH] = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = sizeof(fileName);
            ofn.lpstrFilter = L"All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                sourceFilePath = fileName;
            }
        }
        break;

        case IDC_BUTTONDEST:
        {
            BROWSEINFO bi = {};
            bi.lpszTitle = L"Choose Destination Folder";
            LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

            if (pidl != 0) {
                wchar_t path[MAX_PATH];
                SHGetPathFromIDList(pidl, path);
                IMalloc* imalloc = 0;
                if (SUCCEEDED(SHGetMalloc(&imalloc))) {
                    imalloc->Free(pidl);
                    imalloc->Release();
                }
                destFolderPath = path;
            }
        }
        break;

        case IDC_BUTTONSTART:
        {
            if (!sourceFilePath.empty() && !destFolderPath.empty() && !copying) {
                copying = true;
                std::thread(CopyFileAsync).detach();
            }
        }
        break;

        case IDC_BUTTONPAUSE:
        {
            if (copying) {
                paused = !paused;
                SetWindowText(hwndButtonPause, paused ? L"Старт" : L"Пауза");
            }
        }
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