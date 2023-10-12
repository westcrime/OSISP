#pragma once

#include "resource.h"
#include <Windows.h>
#include <string>

void startMonitoring(int msRefresh);
SystemInfo systemInfo;
HWND workloadText;
HWND mainHwnd;
HWND memoryText;
RECT rectWorkloadOutline;
RECT rectMemoryOutline;
RECT rectWorkload;
RECT rectMemory;
bool needToRedrawRects = true;
double workload = 0.0;
double memory = 0.0;

LPCWSTR string_to_lpcwstr(std::string s);
DWORD WINAPI startMonitoring(LPVOID lpParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);