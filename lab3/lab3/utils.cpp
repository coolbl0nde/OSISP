#include "framework.h"
#include "lab3.h"
#include <string>
#include <thread>
#include <fstream>
#include <commctrl.h>

std::wstring sourceFilePath, destFolderPath;
bool paused = false, copying = false;

void UpdateProgress(HWND hwndProgress, HWND hwndStatusLabel, float percentage, const std::wstring& status) {
    SendMessage(hwndProgress, PBM_SETPOS, (int)(percentage * 100), 0);
    SetWindowText(hwndStatusLabel, status.c_str());
}

void CopyFileAsync(HWND hwndProgress, HWND hwndStatusLabel) {
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
        UpdateProgress(hwndProgress, hwndStatusLabel, progress, status);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    source.close();
    dest.close();

    copying = false;
    UpdateProgress(hwndProgress, hwndStatusLabel, 0.0f, L"");
    MessageBox(NULL, L"Copy completed!", L"Success", MB_OK);
}