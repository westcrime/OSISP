#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include "Archiver.h"
#include "Globals.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    mainHinstance = hInstance;
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Архиватор",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    mainWindowHwnd = hwnd;

    if (hwnd == NULL)
    {
        return 0;
    }

    extractButton = CreateWindowW(L"BUTTON", L"Извлечь", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        100, 100, 100, 100, hwnd, (HMENU)extract, hInstance, NULL);

    if (extractButton == NULL) {
        MessageBox(NULL, L"Создание первой кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        return 1;
    }

    archiveButton = CreateWindow(L"BUTTON", L"Архивировать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 100, 100, 100, hwnd, (HMENU)archive, hInstance, NULL);

    if (archiveButton == NULL) {
        MessageBox(NULL, L"Создание второй кнопки не удалось!", L"Ошибка", MB_ICONERROR);
        return 1;
    }


    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
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
        int buttonSpacing = 10;

        int x1 = (width - 2 * buttonSpacing - 2 * buttonWidth) / 2;
        int x2 = x1 + buttonWidth + buttonSpacing;

        SetWindowPos(extractButton, NULL, x1, height / 2, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(archiveButton, NULL, x2, height / 2, buttonWidth, buttonHeight, SWP_NOZORDER);

        break;
    }
    case WM_COMMAND:
        switch (wParam)
        {
        case extract:
        {
            ShowExtractionWindow(mainWindowHwnd, mainHinstance);
            break;
        }
        case archive:
        {
            ShowArchivationWindow(mainWindowHwnd, mainHinstance);
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