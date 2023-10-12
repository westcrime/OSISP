#pragma once

#include "resource.h"
#include "ArchivateWindow.h"
#include "ExtractWindow.h"

#define extract 1001
#define archive 1002

HWND extractButton;
HWND archiveButton;
HWND mainWindowHwnd;
HINSTANCE mainHinstance;
ArchivateWindow archivateWindow;
ExtractWindow extractWindow;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);