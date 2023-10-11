#pragma once
#include <windows.h>

void ShowExtractionWindow(HWND mainWindow, HINSTANCE hInstance);
void ShowArchivationWindow(HWND mainWindow, HINSTANCE hInstance);
bool compressFile(HWND hwnd, LPCWSTR sourceFilePath, const char* destFilePath);
bool extractFile(HWND hwnd, const char* sourceArchivePath, LPCWSTR destFilePath);
