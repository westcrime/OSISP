#include "ExtractWindow.h"
#include "Globals.h"
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <string>

void ShowExtractionWindow(HWND mainWindow, HINSTANCE hInstance)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = ExtractionWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ExtractWindowClass";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Регистрация класса окна не удалась!", L"Ошибка", MB_ICONERROR);
    }
    HWND ExtractionWindow = CreateWindowW(L"ExtractWindowClass", L"Извлечение файла", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, mainWindow, NULL, GetModuleHandle(NULL), NULL);
    if (ExtractionWindow == NULL) {
        MessageBox(mainWindow, L"Создание нового окна не удалось!", L"Ошибка", MB_ICONERROR);
    }
    else {
        hExEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            200, 185, 200, 30, ExtractionWindow, NULL, hInstance, NULL);

        if (hExEdit == NULL) {
            MessageBox(NULL, L"Создание текстового поля не удалось!", L"Ошибка", MB_ICONERROR);
        }

        hExButtonPath = CreateWindowW(L"BUTTON", L"Выбрать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            410, 185, 100, 30, ExtractionWindow, (HMENU)choosePath, hInstance, NULL);

        if (hExButtonPath == NULL) {
            MessageBox(NULL, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        }

        hExArchivateButtonPath = CreateWindowW(L"BUTTON", L"Извлечь", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            250, 225, 100, 30, ExtractionWindow, (HMENU)extract, hInstance, NULL);

        if (hExButtonPath == NULL) {
            MessageBox(NULL, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        }

        // Показать новое окно
        ShowWindow(ExtractionWindow, SW_SHOW);
        UpdateWindow(ExtractionWindow);
    }
}

LRESULT CALLBACK ExtractionWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
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

        SetWindowPos(hExEdit, NULL, x1, height / 2, editWidth, editHeight, SWP_NOZORDER);
        SetWindowPos(hExButtonPath, NULL, x2, height / 2, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(hExArchivateButtonPath, NULL, x3, height / 2 + 50, buttonWidth, buttonHeight, SWP_NOZORDER);

        break;
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
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szExFilePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"Архивы Gzip (*.gz)\0*.gz\0Все файлы (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                SetWindowText(hExEdit, szExFilePath);
                char filePath[MAX_PATH];
                size_t convertedChars = 0;
                errno_t err = wcstombs_s(&convertedChars, filePath, MAX_PATH, szExFilePath, MAX_PATH - 1);

                if (err != 0) {
                    MessageBox(hwnd, L"Ошибка конвертации строки", L"Архивация", MB_ICONERROR);
                }
                std::string str(filePath);
                if (!str.find(".gz"))
                {
                    MessageBox(hwnd, L"Неверный формат файла", L"Архивация", MB_ICONERROR);
                    break;
                }
            }
            break;
        }
        case extract:
        {
            GetWindowText(hExEdit, szExFilePath, MAX_PATH);
            char filePath[MAX_PATH];
            size_t convertedChars = 0;
            errno_t err = wcstombs_s(&convertedChars, filePath, MAX_PATH, szExFilePath, MAX_PATH - 1);
            if (err != 0) {
                MessageBox(hwnd, L"Ошибка конвертации строки", L"Извлечение", MB_ICONERROR);
                break;
            }
            std::string str(filePath);
            if (!str.find(".gz") || !fileExists(szExFilePath))
            {
                MessageBox(hwnd, L"Неверный формат файла", L"Извлечение", MB_ICONERROR);
                break;
            }
            std::string destPath(filePath);
            size_t found = destPath.find_last_of(".");

            if (found != std::string::npos) {
                destPath = (destPath.substr(0, found) + ".txt").c_str();
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

            if (extractFile(hwnd, filePath, wcstring))
            {
                MessageBox(hwnd, L"Файл успешно извлечен!", L"Извлечение", MB_ICONINFORMATION);
            }
            delete[] wcstring;
            break;
        }
        default:
            break;
        }
    default:
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
}

bool fileExists(LPWSTR filePath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFile = FindFirstFile(filePath, &findFileData);
    if (hFile != INVALID_HANDLE_VALUE) {
        FindClose(hFile);
        return true;
    }
    return false;
}