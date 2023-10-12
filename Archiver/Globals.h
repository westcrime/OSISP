#pragma once
#include <windows.h>

bool compressFile(HWND hwnd, LPCWSTR sourceFilePath, const char* destFilePath);
bool extractFile(HWND hwnd, const char* sourceArchivePath, LPCWSTR destFilePath);
