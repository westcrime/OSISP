#include "ExtractWindow.h"
#include "Globals.h"
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <string>

ExtractWindow::ExtractWindow() { }

ExtractWindow::ExtractWindow(HWND parentWindow, HINSTANCE hInstance)
{
    m_parentWindow = parentWindow;
    m_hInstance = hInstance;
    this->Create(L"Извлечение файла", WS_OVERLAPPEDWINDOW, NULL,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, m_parentWindow, NULL, m_hInstance);
    if (m_hwnd == NULL)
    {
        MessageBox(parentWindow, L"Создание нового окна не удалось!", L"Ошибка", MB_ICONERROR);
        CloseWindow(m_parentWindow);
    }
    else
    {
        m_hEditPath = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            200, 185, 200, 30, m_hwnd, NULL, m_hInstance, NULL);

        if (m_hEditPath == NULL) 
        {
            MessageBox(m_parentWindow, L"Создание текстового поля не удалось!", L"Ошибка", MB_ICONERROR);
            CloseWindow(m_parentWindow);
        }

        m_hEditType = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            250, 240, 100, 30, m_hwnd, NULL, m_hInstance, NULL);

        if (m_hEditType == NULL)
        {
            MessageBox(m_parentWindow, L"Создание текстового поля не удалось!", L"Ошибка", MB_ICONERROR);
            CloseWindow(m_parentWindow);
        }
        SetWindowText(m_hEditType, L"txt");
        m_hStaticLabel = CreateWindowW(
            L"Static",                  // Класс элемента управления "Static"
            L"Напишите расширение файла, которого хотите извлечь",                 // Текст надписи
            WS_CHILD | WS_VISIBLE,      // Стиль окна
            150, 30,                     // Положение (x, y)
            200, 30,                    // Ширина и высота
            m_hwnd,                       // Родительское окно
            (HMENU)NULL,                // Идентификатор элемента управления (может быть NULL)
            GetModuleHandle(NULL),      // Дескриптор экземпляра приложения
            NULL                        // Дополнительные данные (может быть NULL)
        );

        m_hButtonChoosePath = CreateWindowW(L"BUTTON", L"Выбрать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            410, 185, 100, 30, m_hwnd, (HMENU)choosePath, m_hInstance, NULL);

        if (m_hButtonChoosePath == NULL)
        {
            MessageBox(m_parentWindow, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
            CloseWindow(m_parentWindow);
        }

        m_hButtonExtract = CreateWindowW(L"BUTTON", L"Извлечь", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            250, 275, 100, 30, m_hwnd, (HMENU)extract, m_hInstance, NULL);

        if (m_hButtonExtract == NULL) 
        {
            MessageBox(m_parentWindow, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
            CloseWindow(m_parentWindow);
        }
    }
}

LRESULT ExtractWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        int editTypeWidth = 150;
        int editHeight = 30;
        int spacing = 10;

        int x1 = (width - 2 * spacing - 2 * editWidth) / 2 + editWidth / 2 - spacing * 2;
        int x2 = x1 + editWidth + spacing;
        int x3 = width / 2 - 50;
        int x4 = x2 + 50 + spacing;
        int x5_text = x4 - 200 - spacing;

        SetWindowPos(m_hEditPath, NULL, x1, height / 2, editWidth, editHeight, SWP_NOZORDER);
        SetWindowPos(m_hButtonChoosePath, NULL, x2, height / 2, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(m_hEditType, NULL, x4, height / 2 + 50, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(m_hStaticLabel, NULL, x5_text, height / 2 + 50, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(m_hButtonExtract, NULL, x3, height / 2 + 100, buttonWidth, buttonHeight, SWP_NOZORDER);

        break;
    }
    case WM_COMMAND:
        switch (wParam)
        {
        case choosePath:
        {
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = m_hwnd;
            ofn.lpstrFile = m_wfilePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"Архивы Gzip (*.gz)\0*.gz\0Все файлы (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                SetWindowText(m_hEditPath, m_wfilePath);
                char filePath[MAX_PATH];
                size_t convertedChars = 0;
                errno_t err = wcstombs_s(&convertedChars, filePath, MAX_PATH, m_wfilePath, MAX_PATH - 1);

                if (err != 0) {
                    MessageBox(m_hwnd, L"Ошибка конвертации строки", L"Архивация", MB_ICONERROR);
                }
                std::string str(filePath);
                if (!str.find(".gz"))
                {
                    MessageBox(m_hwnd, L"Неверный формат файла", L"Архивация", MB_ICONERROR);
                    break;
                }
            }
            break;
        }
        case extract:
        {
            GetWindowText(m_hEditPath, m_wfilePath, MAX_PATH);
            char filePath[MAX_PATH];
            size_t convertedChars = 0;
            errno_t err = wcstombs_s(&convertedChars, filePath, MAX_PATH, m_wfilePath, MAX_PATH - 1);
            if (err != 0) {
                MessageBox(m_hwnd, L"Ошибка конвертации строки", L"Извлечение", MB_ICONERROR);
                break;
            }
            std::string str(filePath);
            if (!str.find(".gz") || !fileExists(m_wfilePath))
            {
                MessageBox(m_hwnd, L"Неверный формат файла", L"Извлечение", MB_ICONERROR);
                break;
            }
            std::string destPath(filePath);
            size_t found = destPath.find_last_of(".");
            wchar_t type[100];
            GetWindowTextW(m_hEditType, type, 100);
            std::wstring ws(type);
            // your new String
            std::string str_type(ws.begin(), ws.end());
            if (found != std::string::npos) {
                destPath = (destPath.substr(0, found) + "." + str_type).c_str();
            }
            else {
                destPath = destPath.c_str();
            }

            // newsize describes the length of the
            // wchar_t string called wcstring in terms of the number
            // of wide characters, not the number of bytes.
            size_t newsize = destPath.length() + 1;

            // The following creates a buffer large enough to contain
            // the exact number of characters in the original string
            // in the new format. If you want to add more characters
            // to the end of the string, increase the value of newsize
            // to increase the size of the buffer.
            wchar_t* wcstring = new wchar_t[newsize];

            // Convert char* string to a wchar_t* string.
            size_t convertedChars2 = 0;
            mbstowcs_s(&convertedChars2, wcstring, newsize, destPath.c_str(), _TRUNCATE);
            // Display the result and indicate the type of string that it is.

            if (extractFile(m_hwnd, filePath, wcstring))
            {
                MessageBox(m_hwnd, L"Файл успешно извлечен!", L"Извлечение", MB_ICONINFORMATION);
            }
            delete[] wcstring;
            break;
        }
        default:
            break;
        }
    default:
    {
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    }
}

bool ExtractWindow::fileExists(LPWSTR filePath)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFile = FindFirstFile(filePath, &findFileData);
    if (hFile != INVALID_HANDLE_VALUE) {
        FindClose(hFile);
        return true;
    }
    return false;
}
