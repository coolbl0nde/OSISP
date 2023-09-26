#include "helpers.h"

#include <richedit.h>

void SaveToFile(HWND hWnd, const WCHAR* fileName, const WCHAR* content)
{
    std::wofstream file(fileName);
    file << content;
    file.close();
    MessageBox(hWnd, L"Файл успешно сохранен!", L"Уведомление", MB_OK);
}

std::wstring FilterContent(const WCHAR* content)
{
    std::wstring FilteredContent;
    for (int i = 0; content[i] != L'\0'; i++) {
        if (content[i] != L'\n') {
            FilteredContent += content[i];
        }
    }
    return FilteredContent;
}

void HandleSaveAs(HWND hWnd, HWND hEdit)
{
    WCHAR fileName[MAX_PATH] = L"";
    WCHAR szNewContent[10000];
    GetWindowText(hEdit, szNewContent, sizeof(szNewContent) / sizeof(WCHAR));
    std::wstring FilteredContent = FilterContent(szNewContent);

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = sizeof(fileName) / sizeof(WCHAR);
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

    if (GetSaveFileName(&ofn))
    {
        SaveToFile(hWnd, fileName, FilteredContent.c_str());
    }
}

std::wstring HandleOpen(HWND hWnd, HWND hEdit)
{
    WCHAR fileName[MAX_PATH] = L"";
    WCHAR fileContent[10000] = L"";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = sizeof(fileName) / sizeof(WCHAR);
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn))
    {
        std::wifstream file(fileName);
        if (file.is_open())
        {
            file.get(fileContent, sizeof(fileContent) / sizeof(WCHAR), L'\0');
            file.close();

            WCHAR modifiedContent[10000] = L"";
            int j = 0;
            for (int i = 0; fileContent[i] != L'\0'; i++) {
                if (fileContent[i] == L'\n') {
                    modifiedContent[j++] = '\r';
                }

                modifiedContent[j++] = fileContent[i];

                if (fileContent[i] == L'\r') {
                    modifiedContent[j++] = '\n';
                }
            }

            SetWindowText(hEdit, modifiedContent);

            return fileName;
        }
        else
        {
            MessageBox(hWnd, L"Не удалось открыть файл!", L"Ошибка", MB_OK);
        }
    }
}

void ChangeTextColor(HWND hEdit, COLORREF color) {
    // Get the current selection
    CHARRANGE selRange;
    SendMessage(hEdit, EM_EXGETSEL, 0, (LPARAM)&selRange);

    // Check if there's any selected text
    if (selRange.cpMin != selRange.cpMax)
    {
        // Set the desired text color (For example, RGB(255, 0, 0) for red)
        CHARFORMAT2 cf;
        ZeroMemory(&cf, sizeof(cf));
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = color;

        // Set the new CHARFORMAT to the control
        SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }
}