#pragma once
#include <Windows.h>

#define choosePath 1101
#define archivate 1102

HWND hEdit;
HWND hButtonPath;
HWND hArchivateButtonPath;
TCHAR szFilePath[MAX_PATH];


LRESULT CALLBACK ArchivationWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool fileExists(LPWSTR filePath);