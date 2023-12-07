#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 1024

HWND g_hwnd;
static HWND hwndEditInput;
static HWND hwndListBox;
static HWND hwndButtonSend;
SOCKET g_socket;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

DWORD WINAPI ReceiveThread(LPVOID param) {
    char recvbuf[DEFAULT_BUFLEN];

    while (true) {
        int recvResult = recv(g_socket, recvbuf, DEFAULT_BUFLEN, 0);
        if (recvResult > 0) {
            recvbuf[recvResult] = '\0';

            int len = MultiByteToWideChar(CP_UTF8, 0, recvbuf, -1, NULL, 0);
            wchar_t* w_recvbuf = new wchar_t[len];
            MultiByteToWideChar(CP_UTF8, 0, recvbuf, -1, w_recvbuf, len);
            // Получение текста из текстового поля
            wchar_t textBuffer[DEFAULT_BUFLEN];
            GetWindowText(hwndListBox, textBuffer, DEFAULT_BUFLEN);

            // Преобразование текста к объекту std::wstring
            std::wstring currentText = textBuffer;

            // Добавление строки к текущему тексту
            std::wstring additionalText(w_recvbuf);
            currentText += additionalText;
            SendMessage(hwndListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(currentText.c_str()));

            delete[] w_recvbuf;
        }
        else if (recvResult == 0) {
            MessageBox(NULL, L"Connection closed", L"Error", MB_OK | MB_ICONERROR);
            break;
        }
        else {
            MessageBox(NULL, L"Receive failed", L"Error", MB_OK | MB_ICONERROR);
            break;
        }
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    MSG msg;
    HWND hwnd;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"WinSockClient";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"WinSockClient", L"WinSock Client", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        MessageBox(NULL, L"WSAStartup failed", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        MessageBox(NULL, L"getaddrinfo failed", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        g_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (g_socket == INVALID_SOCKET) {
            MessageBox(NULL, L"Error creating socket", L"Error", MB_OK | MB_ICONERROR);
            WSACleanup();
            return 1;
        }

        iResult = connect(g_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(g_socket);
            g_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (g_socket == INVALID_SOCKET) {
        MessageBox(NULL, L"Unable to connect to server!", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    CreateThread(NULL, 0, ReceiveThread, NULL, 0, NULL);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    closesocket(g_socket);
    WSACleanup();

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_CREATE:
        hwndEditInput = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_AUTOHSCROLL, 50, 360, 300, 30, hwnd, NULL, NULL, NULL);
        hwndListBox = CreateWindowEx( 0, L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_DISABLENOSCROLL, 50, 50, 400, 300, hwnd, NULL, NULL,NULL );
        hwndButtonSend = CreateWindowEx(NULL, L"BUTTON", L"Send", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 360, 360, 90, 30, hwnd, (HMENU)1, NULL, NULL);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            wchar_t sendbuf[DEFAULT_BUFLEN];
            GetWindowText(hwndEditInput, sendbuf, DEFAULT_BUFLEN);
            int len = wcslen(sendbuf);
            // Get the required length of the narrow string
            int requiredSize = WideCharToMultiByte(CP_UTF8, 0, sendbuf, -1, NULL, 0, NULL, NULL);

            // Allocate memory for the narrow string
            char* str = new char[requiredSize];

            // Convert the wide string to narrow string
            WideCharToMultiByte(CP_UTF8, 0, sendbuf, -1, str, requiredSize, NULL, NULL);

            
            send(g_socket, str, len, 0);
            SetWindowText(hwndEditInput, L"");
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
