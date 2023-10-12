#pragma once
#include <Windows.h>
#include "BaseWindow.cpp"

#define choosePath 1101
#define archivate 1102

class ArchivateWindow : public BaseWindow<ArchivateWindow>
{
private:
    HWND m_parentWindow;
    HINSTANCE m_hInstance;
    HWND m_hEditPath;
    HWND m_hButtonChoosePath;
    HWND m_hButtonArchivate;
    TCHAR m_wfilePath[MAX_PATH];
public:
    LPCWSTR ClassName() const { return L"ArchivateWindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	ArchivateWindow(HWND parentWindow, HINSTANCE hInstance);
    ArchivateWindow();
	static bool fileExists(LPWSTR filePath);
};