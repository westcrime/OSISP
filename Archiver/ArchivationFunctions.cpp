#include "Globals.h"
#include <zlib.h>
#include <Windows.h>
#include <thread>

bool compressFile(HWND hwnd, LPCWSTR sourceFilePath, const char* destFilePath) {
    HANDLE hSourceFile = CreateFileW(sourceFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSourceFile == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, L"Не удалось открыть исходный файл для чтения", L"Архивация", MB_ICONERROR);
        return false;
    }

    gzFile destFile = gzopen(destFilePath, "wb");
    if (!destFile) 
    {
        MessageBox(hwnd, L"Не удалось открыть файл для записи", L"Архивация", MB_ICONERROR);
        CloseHandle(hSourceFile);
        return false;
    }

    char buffer[1024];
    DWORD bytesRead;

    std::thread writerThread([&]() {
        while (ReadFile(hSourceFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
            if (gzwrite(destFile, buffer, bytesRead) == 0) {
                MessageBox(hwnd, L"Ошибка при записи данных в архив", L"Архивация", MB_ICONERROR);
                gzclose(destFile);
                CloseHandle(hSourceFile);
                return;
            }
        }

        gzclose(destFile);
        CloseHandle(hSourceFile);
        });

    writerThread.join();

    return true;
}

bool extractFile(HWND hwnd, const char* sourceArchivePath, LPCWSTR destFilePath)
{
    gzFile gzFile = gzopen(sourceArchivePath, "rb");
    if (!gzFile) {
        MessageBox(hwnd, L"Не удалось открыть исходный архив для чтения", L"Извлечение", MB_ICONERROR);
        return false;
    }

    HANDLE hFile = CreateFile(destFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, L"Не удалось создать файл для записи", L"Извлечение", MB_ICONERROR);
        gzclose(gzFile);
        return false;
    }

    char buffer[1024];
    int bytesRead;
    DWORD bytesWritten;

    std::thread readerThread([&]() {
        while ((bytesRead = gzread(gzFile, buffer, sizeof(buffer))) > 0) {
            if (!WriteFile(hFile, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten != bytesRead) {
                MessageBox(hwnd, L"Ошибка при записи данных в файл", L"Извлечение", MB_ICONERROR);
                gzclose(gzFile);
                CloseHandle(hFile);
                return;
            }
        }

        gzclose(gzFile);
        CloseHandle(hFile);
        });

    readerThread.join();

    return true;
}