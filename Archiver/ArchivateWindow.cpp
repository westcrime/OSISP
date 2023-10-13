#include "ArchivateWindow.h"
#include "Globals.h"
#include <tchar.h>
#include <windows.h>
#include <string>

#include "ArchivateWindow.h"
#include "Globals.h"
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <string>

ArchivateWindow::ArchivateWindow() {   }

ArchivateWindow::ArchivateWindow(HWND parentWindow, HINSTANCE hInstance)
{
    m_parentWindow = parentWindow;
    m_hInstance = hInstance;
    this->Create(L"Архивация файла", WS_OVERLAPPEDWINDOW, NULL,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, m_parentWindow, NULL, m_hInstance);
    if (m_hwnd == NULL)
    {
        MessageBox(m_parentWindow, L"Создание нового окна не удалось!", L"Ошибка", MB_ICONERROR);
    }
    else
    {
        m_hEditPath = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            200, 185, 200, 30, m_hwnd, NULL, m_hInstance, NULL);

        if (m_hEditPath == NULL)
        {
            MessageBox(m_parentWindow, L"Создание текстового поля не удалось!", L"Ошибка", MB_ICONERROR);
        }

        m_hButtonChoosePath = CreateWindowW(L"BUTTON", L"Выбрать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            410, 185, 100, 30, m_hwnd, (HMENU)choosePath, m_hInstance, NULL);

        if (m_hButtonChoosePath == NULL)
        {
            MessageBox(m_parentWindow, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        }

        m_hButtonArchivate = CreateWindowW(L"BUTTON", L"Архивация", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            250, 225, 100, 30, m_hwnd, (HMENU)archivate, m_hInstance, NULL);

        if (m_hButtonArchivate == NULL)
        {
            MessageBox(m_parentWindow, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        }
    }
}

LRESULT ArchivateWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(m_hwnd, &ps);
        break;
    }
    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        int buttonWidth = 100;
        int buttonHeight = 30;
        int editWidth = 250;
        int editHeight = 30;
        int spacing = 10;

        int x1 = (width - 2 * spacing - 2 * editWidth) / 2 + editWidth / 2;
        int x2 = x1 + editWidth + spacing;
        int x3 = width / 2 - 50;

        SetWindowPos(m_hEditPath, NULL, x1, height / 2, editWidth, editHeight, SWP_NOZORDER);
        SetWindowPos(m_hButtonChoosePath, NULL, x2, height / 2, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(m_hButtonArchivate, NULL, x3, height / 2 + 50, buttonWidth, buttonHeight, SWP_NOZORDER);

        break;
    }
    case WM_COMMAND:
    {
        switch (wParam)
        {
        case choosePath:
        {
            lstrcpyn(m_wfilePath, _T(""), MAX_PATH);
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = m_hwnd;
            ofn.lpstrFile = m_wfilePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                SetWindowText(m_hEditPath, m_wfilePath);
            }
            break;
        }
        case archivate:
        {
            GetWindowText(m_hEditPath, m_wfilePath, MAX_PATH);
            char filePath[MAX_PATH];
            size_t convertedChars = 0;
            errno_t err = wcstombs_s(&convertedChars, filePath, MAX_PATH, m_wfilePath, MAX_PATH - 1);

            if (err != 0) {
                MessageBox(m_hwnd, L"Ошибка конвертации строки", L"Архивация", MB_ICONERROR);
            }
            if (!fileExists(m_wfilePath))
            {
                MessageBox(m_hwnd, L"Неверный формат файла", L"Архивация", MB_ICONERROR);
                break;
            }
            std::string destPath(filePath);
            size_t found = destPath.find_last_of(".");

            if (found != std::string::npos) {
                destPath = (destPath.substr(0, found)).c_str();
            }
            else {
                destPath = destPath.c_str();
            }
            if (compressFile(m_hwnd, m_wfilePath, (destPath + ".gz").c_str()))
            {
                MessageBox(m_hwnd, L"Процесс архивирования успешно запущен!", L"Архивация", MB_ICONINFORMATION);
            }
            break;
        }

        default:
        {
            break;
        }
        }
        break;
    }
    case WM_CLOSE:
    {
        ShowWindow(m_hwnd, SW_HIDE);
        break;
    }
    default:
    {
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    }
}

bool ArchivateWindow::fileExists(LPWSTR filePath)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFile = FindFirstFile(filePath, &findFileData);
    if (hFile != INVALID_HANDLE_VALUE) {
        FindClose(hFile);
        return true;
    }
    return false;
}
