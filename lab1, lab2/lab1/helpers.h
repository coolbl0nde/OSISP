#ifndef HELPERS_H
#define HELPERS_H

#include <windows.h>
#include <string>
#include <fstream>

void SaveToFile(HWND hWnd, const WCHAR* fileName, const WCHAR* content);
std::wstring FilterContent(const WCHAR* content);
void HandleSaveAs(HWND hWnd, HWND hEdit);
std::wstring HandleOpen(HWND hWnd, HWND hEdit);
void ChangeTextColor(HWND hRichEdit, COLORREF color);

#endif