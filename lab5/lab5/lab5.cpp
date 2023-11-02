#include <windows.h>
#include <string>

const wchar_t CLASS_NAME[] = L"Sample Window Class";
const wchar_t WINDOW_NAME[] = L"Registry Manager";

const wchar_t* subkey = L"MyProject";

HWND hKeyEdit;
HWND hValueEdit;
HWND hDataEdit;
HWND hListBox;


HKEY OpenMainKey() {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
        MessageBox(NULL, L"Ошибка открытия ключа.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
    return hKey;
}


void ListSubKeys() {
    HKEY hMainKey = NULL;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hMainKey) == ERROR_SUCCESS) {
        TCHAR    achKey[MAX_PATH];
        TCHAR    achValue[MAX_PATH];
        DWORD    cbName;
        DWORD    cSubKeys = 0, cValues = 0;
        DWORD    cbValueName, cbValueData, valueType;
        BYTE     dataBuffer[512];

        RegQueryInfoKey(hMainKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, &cValues, NULL, NULL, NULL, NULL);

        for (DWORD i = 0; i < cSubKeys; i++) {
            cbName = MAX_PATH;
            if (RegEnumKeyEx(hMainKey, i, achKey, &cbName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)achKey);

                HKEY hSubKey;
                if (RegOpenKeyEx(hMainKey, achKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                    DWORD cSubKeyValues = 0;
                    RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, &cSubKeyValues, NULL, NULL, NULL, NULL);
                    for (DWORD j = 0; j < cSubKeyValues; j++) {
                        cbValueName = MAX_PATH;
                        cbValueData = sizeof(dataBuffer);
                        if (RegEnumValue(hSubKey, j, achValue, &cbValueName, NULL, &valueType, dataBuffer, &cbValueData) == ERROR_SUCCESS) {
                            if (valueType == REG_SZ) {
                                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(std::wstring(achValue) + L": " + (wchar_t*)dataBuffer).c_str());
                            }
                        }
                    }
                    RegCloseKey(hSubKey);
                }
            }
        }
        RegCloseKey(hMainKey);
    }
}

void RefreshListBox() {
    SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
    ListSubKeys();
}

void CreateKey(const wchar_t* keyName) {
    HKEY hMainKey = OpenMainKey();
    HKEY hSubKey;
    if (RegCreateKeyEx(hMainKey, keyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hSubKey, NULL) == ERROR_SUCCESS) {
        MessageBox(NULL, L"Ключ создан.", L"Успех", MB_OK | MB_ICONINFORMATION);
        RegCloseKey(hSubKey);
    }
    else {
        MessageBox(NULL, L"Ошибка создания ключа.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
    RegCloseKey(hMainKey);
}

void SetValue(const wchar_t* keyName, const wchar_t* valueName, const wchar_t* data) {
    HKEY hMainKey = OpenMainKey();
    HKEY hSubKey;

    wchar_t fullPath[256]; // Выделяем достаточно места под полный путь

    wsprintf(fullPath, L"MyProject\\%s", keyName);

    if (RegOpenKeyEx(hMainKey, keyName, 0, KEY_SET_VALUE, &hSubKey) == ERROR_SUCCESS) {
        if (RegSetValueEx(hSubKey, valueName, 0, REG_SZ, (BYTE*)data, (wcslen(data) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS) {
            MessageBox(NULL, L"Значение установлено.", L"Успех", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBox(NULL, L"Ошибка установки значения.", L"Ошибка", MB_OK | MB_ICONERROR);
        }
        RegCloseKey(hSubKey);
    }
    else {
        MessageBox(NULL, L"Ошибка открытия подключа.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
    RegCloseKey(hMainKey);
}

void DeleteKey(const wchar_t* keyName) {
    HKEY hMainKey = OpenMainKey();
    if (RegDeleteKey(hMainKey, keyName) == ERROR_SUCCESS) {
        MessageBox(NULL, L"Ключ удален.", L"Успех", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBox(NULL, L"Ошибка удаления ключа.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
    RegCloseKey(hMainKey);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindow(L"STATIC", L"Key:", WS_CHILD | WS_VISIBLE, 20, 20, 60, 20, hwnd, NULL, NULL, NULL);
        hKeyEdit = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 80, 20, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Value:", WS_CHILD | WS_VISIBLE, 20, 50, 60, 20, hwnd, NULL, NULL, NULL);
        hValueEdit = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 80, 50, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Data:", WS_CHILD | WS_VISIBLE, 20, 80, 60, 20, hwnd, NULL, NULL, NULL);
        hDataEdit = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 80, 80, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"BUTTON", L"Create Key", WS_CHILD | WS_VISIBLE, 20, 110, 150, 30, hwnd, (HMENU)1, NULL, NULL);
        CreateWindow(L"BUTTON", L"Modify Key", WS_CHILD | WS_VISIBLE, 20, 150, 150, 30, hwnd, (HMENU)2, NULL, NULL);
        CreateWindow(L"BUTTON", L"Delete Key", WS_CHILD | WS_VISIBLE, 20, 190, 150, 30, hwnd, (HMENU)3, NULL, NULL);

        hListBox = CreateWindow(L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL, 300, 20, 170, 200, hwnd, NULL, NULL, NULL);

        RefreshListBox();
        break;
    }
    case WM_COMMAND: {
        int len;
        wchar_t keyName[255];
        wchar_t valueName[255];
        wchar_t data[255];

        switch (wParam) {
        case 1:  // Create Key
            len = GetWindowTextW(hKeyEdit, keyName, ARRAYSIZE(keyName));
            if (len > 0) {
                CreateKey(keyName);
                RefreshListBox();
            }
            break;
        case 2:  // Modify Key
            GetWindowTextW(hKeyEdit, keyName, ARRAYSIZE(keyName));
            GetWindowTextW(hValueEdit, valueName, ARRAYSIZE(valueName));
            GetWindowTextW(hDataEdit, data, ARRAYSIZE(data));
            SetValue(keyName, valueName, data);
            RefreshListBox();
            break;
        case 3:  // Delete Key
            len = GetWindowTextW(hKeyEdit, keyName, ARRAYSIZE(keyName));
            if (len > 0) {
                DeleteKey(keyName);
                RefreshListBox();
            }
            break;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        500, 300, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}