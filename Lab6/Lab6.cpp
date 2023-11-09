#include "Lab6.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

    HWND hwnd = CreateWindowExW(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 1300, 700,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    hEditPhilosophersNumber = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_NUMBER,
        10, 10, 100, 30,
        hwnd,
        (HMENU)1,
        NULL,
        NULL
    );

    HWND hPhilosophersNumberText = CreateWindowW(
        L"STATIC",
        L"Введите значение количества философов",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        100, 10, 250, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    if (hEditPhilosophersNumber == NULL) {
        MessageBox(NULL, L"Edit Control Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hButtonStartDinner = CreateWindowExW(
        0,
        L"BUTTON",
        L"Start dinner",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        10, 50, 100, 50,
        hwnd,
        (HMENU)START_DINNER,
        hInstance,
        NULL
    );

    if (hButtonStartDinner == NULL) {
        MessageBox(NULL, L"Button Control Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hEditLog = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY,
        10, 120, 600, 500,
        hwnd,
        NULL,
        NULL,
        NULL
    );

    if (hEditLog == NULL) {
        MessageBox(NULL, L"Edit Control Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
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
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    case WM_COMMAND:
    {
        int command = LOWORD(wParam);
        switch (command)
        {
        case START_DINNER:
        {
            editMutex = CreateMutex(NULL, FALSE, NULL);
            wchar_t buff[256];
            GetWindowText(hEditPhilosophersNumber, buff, 256);
            buff[255] = '\0';
            NUM_PHILOSOPHERS = std::stoi(buff);
            if (NUM_PHILOSOPHERS < 2)
            {
                MessageBox(NULL, L"Неверное количество!", L"Ошибка", MB_ICONERROR);
                break;
            }
            phil_object = new Monitor();
            HANDLE monitoringThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startDinner, NULL, 0, NULL);
            SetWindowTextW(hEditLog, L"");
            if (monitoringThread == NULL) {
                // Обработка ошибки
                return 1;
            }
            break;
        }
        }
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI philosopherThread(LPVOID param)
{
    int c = 0;
    int i = *(int*)param;
    while (c < times) {
        Sleep(100);
        phil_object->take_fork(i);
        Sleep(500);
        phil_object->put_fork(i);
        c++;
    }
    return 0;
}

void PrintMessage(std::wstring message)
{
    wchar_t buff[3000];
    GetWindowTextW(hEditLog, buff, 3000);
    buff[2999] = '\0';
    message = std::wstring(buff) + message + L' | ';
    SetWindowTextW(hEditLog, message.c_str());
}

DWORD WINAPI startDinner(LPVOID param)
{
    HANDLE *thread_id = new HANDLE[NUM_PHILOSOPHERS];
    int *phil = new int[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        phil[i] = i;
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        thread_id[i] = CreateThread(NULL, 0, philosopherThread, &phil[i], 0, NULL);
        WaitForSingleObject(&editMutex, INFINITE);
        PrintMessage(L"Philosopher " + std::to_wstring(i + 1) + L" is thinking...");
        ReleaseMutex(&editMutex);
    }

    WaitForMultipleObjects(NUM_PHILOSOPHERS, thread_id, TRUE, INFINITE);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        CloseHandle(thread_id[i]);
    }
    return 0;
}

Monitor::Monitor()
{
    phcond = new CONDITION_VARIABLE[NUM_PHILOSOPHERS];
    state = new int[NUM_PHILOSOPHERS];
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        state[i] = THINKING;
        InitializeConditionVariable(&phcond[i]);
    }

    InitializeCriticalSection(&condLock);
}

Monitor::~Monitor()
{
    DeleteCriticalSection(&condLock);
}

void Monitor::test(int phnum) {
    int right = (phnum + 1) % NUM_PHILOSOPHERS;
    int left = (phnum - 1) % NUM_PHILOSOPHERS;
    if (state[right] != EATING &&
        state[left] != EATING &&
        state[phnum] == HUNGRY) {
        state[phnum] = EATING;
        WakeConditionVariable(&phcond[phnum]);
    }
}

void Monitor::take_fork(int phnum) {
    EnterCriticalSection(&condLock);

    state[phnum] = HUNGRY;
    test(phnum);

    while (state[phnum] != EATING) {
        SleepConditionVariableCS(&phcond[phnum], &condLock, INFINITE);
    }
    WaitForSingleObject(&editMutex, INFINITE);
    PrintMessage(L"Philosopher " + std::to_wstring(phnum + 1) + L" is eating...");
    ReleaseMutex(&editMutex);
    LeaveCriticalSection(&condLock);
}

void Monitor::put_fork(int phnum) {
    EnterCriticalSection(&condLock);

    state[phnum] = THINKING;
    test((phnum - 1) % NUM_PHILOSOPHERS);
    test((phnum + 1) % NUM_PHILOSOPHERS);

    LeaveCriticalSection(&condLock);
}