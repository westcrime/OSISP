#pragma once
#include <Windows.h>
#include "ArchivateWindow.h"

#define choosePath 1101
#define extract 1102

class ExtractWindow : public BaseWindow<ExtractWindow>
{
private:
	HINSTANCE m_hInstance;
	HWND m_parentWindow;
	HWND m_hEditPath;
	HWND m_hEditType;
	HWND m_hStaticLabel;
	HWND m_hButtonChoosePath;
	HWND m_hButtonExtract;
	TCHAR m_wfilePath[MAX_PATH];
public:
	ExtractWindow(HWND parentWindow, HINSTANCE hInstance);
	ExtractWindow();
	LPCWSTR ClassName() const { return L"ExtractWindowClass"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static bool fileExists(LPWSTR filePath);
};
