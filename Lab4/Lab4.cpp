#ifndef UNICODE
#define UNICODE
#endif 
#if defined(UNICODE)
#define _tstring wstring
#else
#define _tstring string
#endif

#include <windows.h>
#include <thread>
#include <chrono>
#include "SystemInfo.h"
#include "Lab4.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
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
        WS_OVERLAPPEDWINDOW,           // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    mainHwnd = hwnd;

    workloadText = CreateWindowW(
        L"Static",                  // Класс элемента управления "Static"
        L"Процент загруженности процессора: ",                 // Текст надписи
        WS_CHILD | WS_VISIBLE,      // Стиль окна
        100, 100,                     // Положение (x, y)
        300, 200,                    // Ширина и высота
        hwnd,                       // Родительское окно
        (HMENU)NULL,                // Идентификатор элемента управления (может быть NULL)
        GetModuleHandle(NULL),      // Дескриптор экземпляра приложения
        NULL                        // Дополнительные данные (может быть NULL)
    );

    memoryText = CreateWindowW(
        L"Static",                  // Класс элемента управления "Static"
        L"Процент занятой памяти процессора: ",                 // Текст надписи
        WS_CHILD | WS_VISIBLE,      // Стиль окна
        200, 50,                     // Положение (x, y)
        300, 50,                    // Ширина и высота
        hwnd,                       // Родительское окно
        (HMENU)NULL,                // Идентификатор элемента управления (может быть NULL)
        GetModuleHandle(NULL),      // Дескриптор экземпляра приложения
        NULL                        // Дополнительные данные (может быть NULL)
    );

    ShowWindow(hwnd, nCmdShow);
    HANDLE monitoringThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startMonitoring, (LPVOID)100, 0, NULL);
    if (monitoringThread == NULL) {
        // Обработка ошибки
        return 1;
    }

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    CloseHandle(monitoringThread);
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
        int x1 = 50;
        int x2 = 560;
        int y1 = 200;
        int y2 = 250;
        int spacing = 50;

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        // Создаем перо (краску) для черной обводки
        HPEN blackPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // RGB(0, 0, 0) представляет черный цвет
        HGDIOBJ oldPen = SelectObject(hdc, blackPen);

        // Рисуем прямоугольник с черной обводкой
        rectWorkloadOutline = { x1, y1, x2, y2 }; // Координаты и размеры прямоугольника
        Rectangle(hdc, rectWorkloadOutline.left, rectWorkloadOutline.top, rectWorkloadOutline.right, rectWorkloadOutline.bottom);
        // Рисуем прямоугольник с черной обводкой
        rectMemoryOutline = { x1 + (x2 - x1) + spacing, y1, x2 + (x2 - x1) + spacing, y2}; // Координаты и размеры прямоугольника
        Rectangle(hdc, rectMemoryOutline.left, rectMemoryOutline.top, rectMemoryOutline.right, rectMemoryOutline.bottom);

        // Рисуем прямоугольник с черной обводкой
        rectWorkloadGraphOutline = { 10, 10, 510, 110 }; // Координаты и размеры прямоугольника
        Rectangle(hdc, rectWorkloadGraphOutline.left, rectWorkloadGraphOutline.top, rectWorkloadGraphOutline.right, rectWorkloadGraphOutline.bottom);
        // Рисуем прямоугольник с черной обводкой
        rectMemoryGraphOutline = { 550, 10, 1050, 110 }; // Координаты и размеры прямоугольника
        Rectangle(hdc, rectMemoryGraphOutline.left, rectMemoryGraphOutline.top, rectMemoryGraphOutline.right, rectMemoryGraphOutline.bottom);
        if (!(workload_graph_points.size() != 9 || memory_graph_points.size() != 9))
        {
            HDC hdc2 = GetDC(hwnd);
            MoveToEx(hdc2, 10, 110 - workload_graph_points.at(0), NULL);
            for (int i = 1; i <= 9; ++i)
            {
                LineTo(hdc2, 10 + (i - 1) * 50, 110 - workload_graph_points.at(i - 1));
            }
            MoveToEx(hdc2, 550, 110 - memory_graph_points.at(0), NULL);
            for (int i = 1; i <= 9; ++i)
            {
                LineTo(hdc2, 550 + 10 + (i - 1) * 50, 110 - memory_graph_points.at(i - 1));
            }
            ReleaseDC(hwnd, hdc2);
        }

        // Восстанавливаем предыдущее перо и освобождаем созданное
        SelectObject(hdc, oldPen);
        DeleteObject(blackPen);

        HBRUSH greenBrush = CreateSolidBrush(RGB(workload * 2.55, 255 - workload * 2.55, 0));

        rectWorkload = { x1, y1, x1 + lround(workload * 5.6), y2 };
        FillRect(hdc, &rectWorkload, greenBrush);

        // Освобождаем кисть
        DeleteObject(greenBrush);

        greenBrush = CreateSolidBrush(RGB(memory * 2.55, 255 - memory * 2.55, 0));

        rectMemory = { x1 + (x2 - x1) + spacing, y1, x1 + (x2 - x1) + spacing + lround(memory * 5.6), y2 };
        FillRect(hdc, &rectMemory, greenBrush);

        // Освобождаем кисть
        DeleteObject(greenBrush);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        SetWindowPos(workloadText, NULL, width / 2 - 500, height / 2 + 50, 400, 100, SWP_NOZORDER);
        SetWindowPos(memoryText, NULL, width / 2 + 100, height / 2 + 50, 400, 100, SWP_NOZORDER);
    }
    case WM_COMMAND:
    {
        switch (wParam)
        {

        default:
        {
            break;
        }
        }
    }
    default:
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
}

// Функция потока
DWORD WINAPI startMonitoring(LPVOID lpParam)
{
    int msRefresh = (int)lpParam;
    int iters = 0;
    while (true) {
        iters++;
        double workload_ = workload;
        double memory_ = memory;
        HANDLE t1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SystemInfo::GetWorkloadInfo, (LPVOID)&workload_, 0, NULL);
        HANDLE t2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SystemInfo::GetMemoryInfo, (LPVOID)&memory_, 0, NULL);

        // Ожидание завершения потоков
        WaitForSingleObject(t1, INFINITE);
        WaitForSingleObject(t2, INFINITE);

        // Закрытие дескрипторов потоков
        CloseHandle(t1);
        CloseHandle(t2);

        workload = workload_;
        memory = memory_;
        std::_tstring workloadStr(L"Процент загруженности процессора: " + std::to_wstring(workload) + L" %");
        std::_tstring memoryStr(L"Процент занятой памяти: " + std::to_wstring(memory) + L" %");
        SetWindowTextW(workloadText, workloadStr.c_str());
        SetWindowTextW(memoryText, memoryStr.c_str());
        if (iters == 10)
        {
            needToRedrawRects = true;
            InvalidateRect(mainHwnd, NULL, TRUE);
            needToRedrawRects = false;
            iters = 0;
            memory_graph_points.push_back(memory);
            if (memory_graph_points.size() == 10)
            {
                memory_graph_points.erase(memory_graph_points.begin());
            }
            workload_graph_points.push_back(workload);
            if (workload_graph_points.size() == 10)
            {
                workload_graph_points.erase(workload_graph_points.begin());
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(msRefresh));
    }
}