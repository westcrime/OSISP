#include "Globals.h"
#include <zlib.h>
#include <Windows.h>

bool compressFile(HWND hwnd, LPCWSTR sourceFilePath, const char* destFilePath) {
    HANDLE hSourceFile = CreateFileW(sourceFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSourceFile == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, L"�� ������� ������� �������� ���� ��� ������", L"���������", MB_ICONERROR);
        return false;
    }

    gzFile destFile = gzopen(destFilePath, "wb");
    if (!destFile) {
        MessageBox(hwnd, L"�� ������� ������� ���� ��� ������", L"���������", MB_ICONERROR);
        CloseHandle(hSourceFile);
        return false;
    }

    char buffer[1024];
    DWORD bytesRead;

    while (ReadFile(hSourceFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        if (gzwrite(destFile, buffer, bytesRead) == 0) {
            MessageBox(hwnd, L"������ ��� ������ ������ � �����", L"���������", MB_ICONERROR);
            gzclose(destFile);
            CloseHandle(hSourceFile);
            return false;
        }
    }

    gzclose(destFile);
    CloseHandle(hSourceFile);

    return true;
}

bool extractFile(HWND hwnd, const char* sourceArchivePath, LPCWSTR destFilePath)
{
    gzFile gzFile = gzopen(sourceArchivePath, "rb");
    if (!gzFile) {
        MessageBox(hwnd, L"�� ������� ������� �������� ����� ��� ������", L"����������", MB_ICONERROR);
        return false;
    }

    HANDLE hFile = CreateFile(destFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, L"�� ������� ������� ���� ��� ������", L"����������", MB_ICONERROR);
        gzclose(gzFile);
        return false;
    }

    char buffer[1024];
    int bytesRead;
    DWORD bytesWritten;

    while ((bytesRead = gzread(gzFile, buffer, sizeof(buffer))) > 0) {
        if (!WriteFile(hFile, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten != bytesRead) {
            MessageBox(hwnd, L"������ ��� ������ ������ � ����", L"����������", MB_ICONERROR);
            gzclose(gzFile);
            CloseHandle(hFile);
            return false;
        }
    }

    gzclose(gzFile);
    CloseHandle(hFile);

    return true;
}