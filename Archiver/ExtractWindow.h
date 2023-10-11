#pragma once
#include <Windows.h>

#define choosePath 1101
#define extract 1102

HWND hExEdit;
HWND hExButtonPath;
HWND hExArchivateButtonPath;
TCHAR szExFilePath[MAX_PATH];


LRESULT CALLBACK ExtractionWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool fileExists(LPWSTR filePath);