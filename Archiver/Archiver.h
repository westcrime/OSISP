#pragma once

#include "resource.h"

#define extract 1001
#define archive 1002

HWND extractButton;
HWND archiveButton;
HWND mainWindowHwnd;
HINSTANCE mainHinstance;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);