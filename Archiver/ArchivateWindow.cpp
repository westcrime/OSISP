#include "ArchivateWindow.h"
#include "Globals.h"
#include <tchar.h>
#include <windows.h>
#include <string>

void ShowArchivationWindow(HWND mainWindow, HINSTANCE hInstance)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = ArchivationWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ArchivateWindowClass";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Регистрация класса окна не удалась!", L"Ошибка", MB_ICONERROR);
    }
    HWND archivationWindow = CreateWindowW(L"ExtractWindowClass", L"Архивация файла", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, mainWindow, NULL, GetModuleHandle(NULL), NULL);
    if (archivationWindow == NULL) {
        MessageBox(mainWindow, L"Создание нового окна не удалось!", L"Ошибка", MB_ICONERROR);
    }
    else {
        hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            200, 185, 200, 30, archivationWindow, NULL, hInstance, NULL);

        if (hEdit == NULL) {
            MessageBox(NULL, L"Создание текстового поля не удалось!", L"Ошибка", MB_ICONERROR);
        }

        hButtonPath = CreateWindowW(L"BUTTON", L"Выбрать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            410, 185, 100, 30, archivationWindow, (HMENU)choosePath, hInstance, NULL);

        if (hButtonPath == NULL) {
            MessageBox(NULL, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        }

        hArchivateButtonPath = CreateWindowW(L"BUTTON", L"Архивировать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            250, 225, 100, 30, archivationWindow, (HMENU)archivate, hInstance, NULL);

        if (hButtonPath == NULL) {
            MessageBox(NULL, L"Создание кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        }

        // Показать новое окно
        ShowWindow(archivationWindow, SW_SHOW);
        UpdateWindow(archivationWindow);
    }
}

LRESULT CALLBACK ArchivationWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        SetWindowPos(hEdit, NULL, x1, height / 2, editWidth, editHeight, SWP_NOZORDER);
        SetWindowPos(hButtonPath, NULL, x2, height / 2, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(hArchivateButtonPath, NULL, x3, height / 2 + 50, buttonWidth, buttonHeight, SWP_NOZORDER);

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
            ofn.lpstrFile = szFilePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                SetWindowText(hEdit, szFilePath);
            }
            break;
        }
        case archivate:
        {
            GetWindowText(hEdit, szFilePath, MAX_PATH);
            char filePath[MAX_PATH];
            size_t convertedChars = 0;
            errno_t err = wcstombs_s(&convertedChars, filePath, MAX_PATH, szFilePath, MAX_PATH - 1);

            if (err != 0) {
                MessageBox(hwnd, L"Ошибка конвертации строки", L"Архивация", MB_ICONERROR);
            }
            if (!fileExists(szFilePath))
            {
                MessageBox(hwnd, L"Неверный формат файла", L"Архивация", MB_ICONERROR);
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
            if (compressFile(hwnd, szFilePath, (destPath + ".gz").c_str()))
            {
                MessageBox(hwnd, L"Файл успешно архивирован!", L"Архивация", MB_ICONINFORMATION);
            }
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