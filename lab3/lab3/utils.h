#pragma once

#include <windows.h>
#include <fstream>

void UpdateProgress(HWND hwndProgress, HWND hwndStatusLabel, float percentage, const std::wstring& status);
void CopyFileAsync(HWND hwndProgress, HWND hwndStatusLabel, std::wstring& sourceFilePath, std::wstring& destFolderPath, bool& paused, bool& copying);