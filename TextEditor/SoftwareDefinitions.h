#pragma once
#include <string>
#include <windows.h>

#define OnMenuClicked 1
#define OnOpenFileClicked 2
#define OnSaveFileClicked 3
#define OnSaveAsFileClicked 4
#define OnCloseEditorClicked 5
#define IDC_EDIT 6
#define HOTKEY_CTRL_S 7
#define HOTKEY_CTRL_O 8
#define OnChangeFontClicked 9
#define OnChangeThemeClicked 10
#define OnCopyTextClicked 11
#define OnCutTextClicked 12
#define OnInsertTextClicked 13

HWND hEditControl;
std::wstring currFilePath;
std::wstring THEME(L"Light");
bool SAVED = true;
HFONT g_hFont; // Дескриптор шрифта
COLORREF bgColor;
COLORREF txtColor;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

void MainWindAddMenus(HWND hwnd);
void MainWindAddWidgets(HWND hwnd);

std::wstring GetFilenameFromPath(std::wstring path);
void SaveTextToFile(const HWND& hwnd);
bool AskForExit();
void DeleteStarFromName(const HWND& hwnd);
void ChangeFont();
void ChangeTheme();