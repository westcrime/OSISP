#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <wchar.h>
#include <fstream>
#include "SoftwareDefinitions.h"
#include <string>
#include <atlstr.h>
#include <vector>
#include <iostream>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    bgColor = RGB(255, 255, 255);
    txtColor = RGB(0, 0, 0);

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(bgColor);

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"TextEditor",  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    g_hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    SendMessage(hEditControl, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (g_hFont)
    {
        DeleteObject(g_hFont);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    wchar_t msg[32];
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == IDC_EDIT)
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                if (SAVED)
                {
                    SAVED = false;
                    wchar_t oldName[100];
                    GetWindowTextW(hwnd, oldName, 100);
                    std::wstring newName(oldName);
                    SetWindowTextW(hwnd, newName.append(L"*").c_str());
                }
            }
        }
        switch (wParam)
        {
        case OnCopyTextClicked:
        {
            // Отправляем сообщение WM_COPY для копирования выделенного текста в буфер обмена
            SendMessage(hEditControl, WM_COPY, 0, 0);
            break;
        }
        case OnCutTextClicked:
        {
            // Отправляем сообщение WM_CUT для вырезания выделенного текста в буфер обмена
            SendMessage(hEditControl, WM_CUT, 0, 0);
            SAVED = false;
            break;
        }
        case OnInsertTextClicked:
        {
            // Отправляем сообщение WM_PASTE для вставки текста из буфера обмена
            SendMessage(hEditControl, WM_PASTE, 0, 0);
            SAVED = false;
            break;
        }
        case OnChangeFontClicked:
        {
            ChangeFont();
            break;
        }
        case OnChangeThemeClicked:
        {
            ChangeTheme();
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }
        case OnOpenFileClicked:
        {
            wchar_t buff[100];

            OPENFILENAME ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = buff;
            ofn.nMaxFile = sizeof(buff);
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrDefExt = L"txt";
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                wchar_t textBuffer[1024];

                currFilePath = buff;
                auto name = GetFilenameFromPath(currFilePath);
                auto newNameOfWnd = std::wstring(L"Text Editor - ").append(name);

                std::wifstream outFile((LPWSTR)currFilePath.c_str());
                if (outFile.is_open()) {
                    outFile >> textBuffer;
                    outFile.close();
                    SetWindowTextW(hEditControl, textBuffer);
                    SetWindowTextW(hwnd, newNameOfWnd.c_str());
                    SAVED = true;
                }
                else {
                    MessageBox(hwnd, L"Failed to open file!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }
        case OnSaveAsFileClicked:
        {
            SaveTextToFile(hwnd);
            SAVED = true;
            DeleteStarFromName(hwnd);
            break;
        }
        case OnSaveFileClicked:
        {
            if (currFilePath.empty())
            {
                SaveTextToFile(hwnd);
            }
            else
            {
                wchar_t textBuffer[1024];
                GetWindowTextW(hEditControl, textBuffer, sizeof(textBuffer) / sizeof(textBuffer[0]));

                std::wofstream outFile(currFilePath);
                if (outFile.is_open()) {
                    outFile << textBuffer;
                    outFile.close();
                    MessageBox(hwnd, L"File saved successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
                    SAVED = true;
                    DeleteStarFromName(hwnd);
                }
                else {
                    MessageBox(hwnd, L"Failed to save file!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }
        case OnCloseEditorClicked:
        {
            if (AskForExit())
            {
                return 0;
            }
            break;
        }
        }
        break;
    }
    case WM_CTLCOLOREDIT:
    {
        HDC hdcEdit = (HDC)wParam;
        SetBkColor(hdcEdit, bgColor);
        SetTextColor(hdcEdit, txtColor);
        return (LRESULT)CreateSolidBrush(bgColor);
        break;  
    }
    case WM_SIZE:
    {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        // Установите новый размер окна
        SetWindowPos(hEditControl, NULL, 1, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);

        break;
    }
    case WM_CREATE:
    {
        MainWindAddMenus(hwnd);
        MainWindAddWidgets(hwnd);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HBRUSH hBrush = CreateSolidBrush(bgColor);
        FillRect(hdc, &ps.rcPaint, hBrush);
        DeleteObject(hBrush);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
    {
        if (AskForExit())
        {
            return 0;
        }
        break;
    }
    case WM_HOTKEY:
    {
        int id = wParam;
        // Проверяем, была ли нажата клавиша Ctrl
        switch (id) 
        {
            // Проверяем, была ли нажата клавиша "S"
        case HOTKEY_CTRL_S:
        {
            // Обработка нажатия Ctrl + S
            if (currFilePath.empty())
            {
                SaveTextToFile(hwnd);
            }
            else
            {
                wchar_t textBuffer[1024];
                GetWindowTextW(hEditControl, textBuffer, sizeof(textBuffer) / sizeof(textBuffer[0]));

                std::wofstream outFile(currFilePath);
                if (outFile.is_open()) {
                    outFile << textBuffer;
                    outFile.close();
                    MessageBox(hwnd, L"File saved successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
                    SAVED = true;
                    DeleteStarFromName(hwnd);
                }
                else {
                    MessageBox(hwnd, L"Failed to save file!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }
        case HOTKEY_CTRL_O:
        {
            // Обработка нажатия Ctrl + O
            wchar_t buff[100];

            OPENFILENAME ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = buff;
            ofn.nMaxFile = sizeof(buff);
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrDefExt = L"txt";
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                wchar_t textBuffer[1024];

                currFilePath = buff;
                auto name = GetFilenameFromPath(currFilePath);
                auto newNameOfWnd = std::wstring(L"Text Editor - ").append(name);

                std::wifstream outFile((LPWSTR)currFilePath.c_str());
                if (outFile.is_open()) {
                    outFile >> textBuffer;
                    outFile.close();
                    SetWindowTextW(hEditControl, textBuffer);
                    SetWindowTextW(hwnd, newNameOfWnd.c_str());
                    SAVED = true;
                }
                else {
                    MessageBox(hwnd, L"Failed to open file!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }
        }
        break;
    }
    default:
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
}
    

void MainWindAddMenus(HWND hwnd)
{
    HMENU RootMenu = CreateMenu();
    HMENU FileMenu = CreateMenu();
    HMENU ViewMenu = CreateMenu();
    HMENU SubViewMenu = CreateMenu();
    HMENU SubMenu = CreateMenu();

    AppendMenuW(SubMenu, MF_STRING, OnOpenFileClicked, L"Open");
    AppendMenuW(SubMenu, MF_STRING, OnSaveAsFileClicked, L"Save As");
    AppendMenuW(SubMenu, MF_STRING, OnSaveFileClicked, L"Save");
    AppendMenuW(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenuW(SubMenu, MF_STRING, OnCopyTextClicked, L"Copy Text");
    AppendMenuW(SubMenu, MF_STRING, OnCutTextClicked, L"Cut Text");
    AppendMenuW(SubMenu, MF_STRING, OnInsertTextClicked, L"Insert Text");
    AppendMenuW(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenuW(SubMenu, MF_STRING, OnCloseEditorClicked, L"Close Editor");

    AppendMenuW(SubViewMenu, MF_STRING, OnChangeFontClicked, L"Change font");
    AppendMenuW(SubViewMenu, MF_STRING, OnChangeThemeClicked, L"Change theme");

    AppendMenu(FileMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
    AppendMenu(ViewMenu, MF_POPUP, (UINT_PTR)SubViewMenu, L"View");

    AppendMenuW(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
    AppendMenuW(RootMenu, MF_POPUP, (UINT_PTR)SubViewMenu, L"View");

    RegisterHotKey(hwnd, HOTKEY_CTRL_S, MOD_CONTROL, 'S');
    RegisterHotKey(hwnd, HOTKEY_CTRL_O, MOD_CONTROL, 'O');

    SetMenu(hwnd, RootMenu);
}

void MainWindAddWidgets(HWND hwnd)
{
    // Получите размер и позицию окна в экранных координатах
    RECT windowRect;
    GetWindowRect(hwnd, &windowRect);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // windowWidth и windowHeight содержат ширину и высоту окна
    // windowRect.left и windowRect.top содержат координаты верхнего левого угла 

    hEditControl = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"", WS_CHILD | WS_VISIBLE | ES_LEFT |
        WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
        1, 0, windowWidth - 20, windowHeight - 20, hwnd, (HMENU)IDC_EDIT, NULL, NULL);
}

std::wstring GetFilenameFromPath(std::wstring path)
{
    // Найти последний символ разделителя пути
    size_t found = path.find_last_of(L"\\/");

    if (found != std::wstring::npos) {
        // Извлечь имя файла после последнего разделителя пути
        std::wstring fileName = path.substr(found + 1);

        // вернуть имя файла
        return fileName;
    }
    else {
        // Если разделитель не найден, это может быть имя файла без пути
        // Вывести всю строку как имя файла
        return path;
    }
}

void SaveTextToFile(const HWND& hwnd)
{
    currFilePath.clear();
    wchar_t buff[100];

    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = buff;
    ofn.nMaxFile = sizeof(buff);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileName(&ofn))
    {
        wchar_t textBuffer[1024];
        currFilePath = buff;
        auto name = GetFilenameFromPath(currFilePath);
        auto newNameOfWnd = std::wstring(L"Text Editor - ").append(name);

        GetWindowTextW(hEditControl, textBuffer, sizeof(textBuffer) / sizeof(textBuffer[0]));

        std::wofstream outFile(buff);
        if (outFile.is_open()) {
            outFile << textBuffer;
            outFile.close();
            MessageBox(hwnd, L"File saved successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
            SetWindowTextW(hwnd, newNameOfWnd.c_str());
            SAVED = true;
            DeleteStarFromName(hwnd);
        }
        else {
            MessageBox(hwnd, L"Failed to save file!", L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

bool AskForExit()
{
    if (!SAVED)
    {
        int msgboxID = MessageBox(
            NULL,
            L"Do you really want to exit? You have unsaved changes",
            L"Exit",
            MB_ICONEXCLAMATION | MB_YESNO);

        if (msgboxID == IDYES)
        {
            PostQuitMessage(0);
            return true;
        }
        return false;
    }
    else
    {
        PostQuitMessage(0);
        return true;
    }
}

void DeleteStarFromName(const HWND& hwnd)
{
    wchar_t oldName[100];
    GetWindowTextW(hwnd, oldName, 100);
    std::wstring newName(oldName);
    // Поиск и удаление символа "*"
    size_t pos = newName.find(L"*");
    while (pos != std::wstring::npos) {
        newName.erase(pos, 1); // Удаление 1 символа, начиная с позиции pos
        pos = newName.find(L"*", pos); // Поиск следующего символа "*"
    }
    SetWindowTextW(hwnd, newName.c_str());
}

void ChangeFont()
{
    CHOOSEFONT cf = { sizeof(CHOOSEFONT) };
    LOGFONT lf = { 0 };

    cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
    cf.lpLogFont = &lf;

    if (ChooseFont(&cf))
    {
        if (g_hFont)
        {
            DeleteObject(g_hFont);
        }

        g_hFont = CreateFontIndirect(&lf);
        SendMessage(hEditControl, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    }
}

void ChangeTheme()
{
    if (THEME == L"Light")
    {
        bgColor = RGB(0, 0, 0);
        txtColor = RGB(255, 255, 255);
        THEME = L"Dark";
    }
    else
    {
        bgColor = RGB(255, 255, 255);
        txtColor = RGB(0, 0, 0);
        THEME = L"Light";
    }
}