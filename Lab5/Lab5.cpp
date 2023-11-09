#ifndef UNICODE
#define UNICODE
#endif 
#include "Lab5.h"
#include <strsafe.h>
#include <tchar.h>
#include <locale>
#include <codecvt>
#include <atlstr.h>
#include <atlconv.h>
/*
Разработать утилиту для создания и управления
реестровыми записями Windows, включая создание,
изменение и удаление ключей и значений.
*/

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

    hEditCreateKeyName = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 10, 300, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    HWND hwndCreateKeyNameText = CreateWindow(
        L"STATIC",
        L"Введите имя ключа для создания/изменения", 
        WS_VISIBLE | WS_CHILD | SS_LEFT, 
        320, 10, 270, 30,  
        hwnd,           
        NULL,                
        hInstance,           
        NULL                  
    );

    hEditCreateKeyValue = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 50, 300, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    HWND hwndCreateValueText = CreateWindow(
        L"STATIC",
        L"Введите имя значения для ключа",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        320, 50, 250, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    hEditCreateKeyValueData = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 90, 300, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    HWND hwndCreateValueDataText = CreateWindow(
        L"STATIC",
        L"Введите значение для данных значения",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        320, 90, 250, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    hComboBoxType = CreateWindow(
        L"COMBOBOX",            
        NULL,                   
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS,
        10, 130, 200, 200,      
        hwnd, NULL, hInstance, NULL
    );

    // Добавление элементов в комбо-бокс
    SendMessage(hComboBoxType, CB_ADDSTRING, 0, (LPARAM)L"REG_SZ");
    SendMessage(hComboBoxType, CB_ADDSTRING, 0, (LPARAM)L"REG_DWORD");
    SendMessage(hComboBoxType, CB_ADDSTRING, 0, (LPARAM)L"REG_QWORD");

    // Выбор первого элемента в комбо-боксе
    SendMessage(hComboBoxType, CB_SETCURSEL, 0, 0);

    hButtonCreateKey = CreateWindowEx(
        0,
        L"BUTTON",
        L"Создать/изменить ключ",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        220, 130, 220, 50,
        hwnd,
        (HMENU)CREATE_KEY,
        hInstance,
        NULL
    );

    hEditDeleteKeyName = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        610, 10, 300, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    HWND hwndDeleteKeyNameText = CreateWindow(
        L"STATIC",
        L"Введите имя ключа для удаления",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        920, 10, 270, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    hButtonDeleteKey = CreateWindowEx(
        0,
        L"BUTTON",
        L"Удалить ключ",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        610, 50, 120, 50,
        hwnd,
        (HMENU)DELETE_KEY,
        hInstance,
        NULL
    );

    hEditDeleteValueName = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        610, 110, 300, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    HWND hwndDeleteValueNameText = CreateWindow(
        L"STATIC",
        L"Введите имя значения ключа для удаления",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        920, 110, 300, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    hButtonDeleteValue = CreateWindowEx(
        0,
        L"BUTTON",
        L"Удалить значение",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        610, 150, 150, 50,
        hwnd,
        (HMENU)DELETE_VALUE,
        hInstance,
        NULL
    );

    hEditFindKeyName = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 340, 300, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    HWND hwndFindKeyNameText = CreateWindow(
        L"STATIC",
        L"Введите имя ключа для поиска",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        320, 340, 270, 30,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    hButtonFindKey = CreateWindowEx(
        0,
        L"BUTTON",
        L"Найти ключ",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        10, 380, 120, 50,
        hwnd,
        (HMENU)FIND_KEY,
        hInstance,
        NULL
    );

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
        case CREATE_KEY:
        {
            LPWSTR name = new WCHAR[GetWindowTextLength(hEditCreateKeyName) + 1];
            LPWSTR value = new WCHAR[GetWindowTextLength(hEditCreateKeyValue) + 1];
            LPWSTR data = new WCHAR[GetWindowTextLength(hEditCreateKeyValueData) + 1];
            GetWindowText(hEditCreateKeyName, name, GetWindowTextLength(hEditCreateKeyName) + 1);
            GetWindowText(hEditCreateKeyValue, value, GetWindowTextLength(hEditCreateKeyValue) + 1);
            GetWindowText(hEditCreateKeyValueData, data, GetWindowTextLength(hEditCreateKeyValueData) + 1);
            if (CreateKey(name, value, data))
            {
                MessageBox(hwnd, L"Ключ успешно создан в реестре.", L"Успех", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(hwnd, L"Ошибка при создании ключа в реестре.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
            delete[] name;
            delete[] value;
            delete[] data;
            break;
        }
        case DELETE_KEY:
        {
            LPWSTR name = new WCHAR[GetWindowTextLength(hEditDeleteKeyName) + 1];
            GetWindowText(hEditDeleteKeyName, name, GetWindowTextLength(hEditDeleteKeyName) + 1);
            if (DeleteKey(name))
            {
                MessageBox(hwnd, L"Ключ успешно удален в реестре.", L"Успех", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(hwnd, L"Ошибка при удалении ключа в реестре.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
            delete[] name;
            break;
        }
        case DELETE_VALUE:
        {
            LPWSTR name = new WCHAR[GetWindowTextLength(hEditDeleteValueName) + 1];
            GetWindowText(hEditDeleteValueName, name, GetWindowTextLength(hEditDeleteValueName) + 1);
            if (DeleteValue(name))
            {
                MessageBox(hwnd, L"Значение успешно удален в реестре.", L"Успех", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(hwnd, L"Ошибка при удалении значения в реестре.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
            delete[] name;
            break;
        }
        case FIND_KEY:
        {
            LPWSTR name = new WCHAR[GetWindowTextLength(hEditFindKeyName) + 1];
            GetWindowText(hEditFindKeyName, name, GetWindowTextLength(hEditFindKeyName) + 1);
            std::wstring result = FindKey(name);
            if (result.empty())
            {
                MessageBox(hwnd, L"Ключ не был найден в реестре.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
            else
            {
                std::wstring mywstring(name);
                result = L"Ключ с именем " + mywstring + L" имеет данные:\n" + result;
                MessageBox(hwnd, result.c_str(), L"Успех", MB_OK | MB_ICONINFORMATION);
            }
            delete[] name;
            break;
        }
        }
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
    LPTSTR lpEnd;
    LONG lResult;
    DWORD dwSize;
    TCHAR szName[MAX_PATH];
    HKEY hKey;
    FILETIME ftWrite;

    // First, see if we can delete the key without having
    // to recurse.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS)
        return TRUE;

    lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

    if (lResult != ERROR_SUCCESS)
    {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found.\n");
            return TRUE;
        }
        else {
            printf("Error opening key.\n");
            return FALSE;
        }
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != TEXT('\\'))
    {
        *lpEnd = TEXT('\\');
        lpEnd++;
        *lpEnd = TEXT('\0');
    }

    // Enumerate the keys

    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
        NULL, NULL, &ftWrite);

    if (lResult == ERROR_SUCCESS)
    {
        do {

            *lpEnd = TEXT('\0');
            StringCchCat(lpSubKey, MAX_PATH * 2, szName);

            if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
                break;
            }

            dwSize = MAX_PATH;

            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                NULL, NULL, &ftWrite);

        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = TEXT('\0');

    RegCloseKey(hKey);

    // Try again to delete the key.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS)
        return TRUE;

    return FALSE;
}

//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnode(HKEY hKeyRoot, LPCTSTR lpSubKey)
{
    TCHAR szDelKey[MAX_PATH * 2];

    StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
    return RegDelnodeRecurse(hKeyRoot, szDelKey);

}

std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

bool CreateKey(std::wstring keyName, std::wstring value, std::wstring data)
{
    if (value.empty())
    {
        return false;
    }

    HKEY hKey;
    LPCWSTR keyPath;
    TCHAR* dst = new TCHAR[256];
    if (!keyName.empty())
    {
        //setup converter
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;

        //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
        std::string converted_str = converter.to_bytes(keyName);
        converted_str = "Software\\Lab5\\" + converted_str;
        //As much as we'd love to, we can't use memcpy() because
        //sizeof(TCHAR)==sizeof(char) may not be true:
        std::copy(converted_str.begin(), converted_str.end(), dst);
        dst[converted_str.size()] = '\0';
    }
    else
    {
        std::string str = "Software\\Lab5";
        std::copy(str.begin(), str.end(), dst);
        dst[str.size()] = '\0';
    }
    LPCWSTR valueName = value.c_str();
    LPCWSTR valueData = data.c_str();

    int selectedIndex = SendMessage(hComboBoxType, CB_GETCURSEL, 0, 0);
    if (selectedIndex == CB_ERR) 
    {
        return false;
    }

    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, dst, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    if (result == ERROR_SUCCESS) 
    {
        if (selectedIndex == 0)
        {
            result = RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)valueData, (wcslen(valueData) + 1) * sizeof(WCHAR));

            if (result != ERROR_SUCCESS)
            {
                return false;
            }

            RegCloseKey(hKey);
            return true;
        }
        if (selectedIndex == 1)
        {
            DWORD dwValue = wcstoul(valueData, NULL, 10);
            result = RegSetValueEx(hKey, valueName, 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(DWORD));

            if (result != ERROR_SUCCESS)
            {
                return false;
            }

            RegCloseKey(hKey);
            return true;
        }
        if (selectedIndex == 2)
        {
            ULONGLONG qwValue = _wcstoui64(valueData, NULL, 10);
            result = RegSetValueEx(hKey, valueName, 0, REG_QWORD, (const BYTE*)&qwValue, sizeof(ULONGLONG));

            if (result != ERROR_SUCCESS)
            {
                return false;
            }

            RegCloseKey(hKey);
            return true;
        }
    }
    else {
        return false;
    }
}

bool DeleteValue(std::wstring name)
{
    if (name.empty())
    {
        return false;
    }
    HKEY hKey = NULL;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Lab5"), 0L, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        TCHAR* dst = W2T((LPWSTR)name.c_str());
        if (RegDeleteValue(hKey, dst) == ERROR_SUCCESS)
        {

            return true;
        }
    }
    else
    {
        return false;
    }
}

bool DeleteKey(std::wstring name)
{
    HKEY hKey;

    if (name.empty())
    {
        return false;
    }
    std::wstring new_name(L"Software\\Lab5\\");
    new_name += name;

    return RegDelnode(HKEY_CURRENT_USER, new_name.c_str());
}

std::wstring FindKey(std::wstring name)
{
    HKEY hKey = HKEY_CURRENT_USER;  // Корневой ключ, в котором находится ключ
    std::wstring keyName = L"Software\\Lab5";  // Путь к ключу
    std::wstring s1 = std::wstring(keyName);
    std::wstring s2 = std::wstring(L"\\");
    std::wstring s3 = std::wstring(name);

    std::wstring subkeyName = (s1 + s2 + s3);


    LONG result = RegOpenKeyExW(hKey, subkeyName.c_str(), 0, KEY_READ, &hKey);

    if (result == ERROR_SUCCESS) {
        // Открытие ключа прошло успешно
        DWORD valueIndex = 0;  // Индекс значения
        WCHAR valueName[256];  // Максимальная длина имени значения
        DWORD valueNameSize = 256;
        DWORD dataType;
        BYTE data[256];  // Максимальный размер данных значения
        DWORD dataSize = 256;
        std::wstring answer;

        while (true) {
            result = RegEnumValueW(hKey, valueIndex, valueName, &valueNameSize, NULL, &dataType, data, &dataSize);

            if (result == ERROR_NO_MORE_ITEMS) {
                // Все значения перечислены
                break;
            }

            if (result == ERROR_SUCCESS) {
                answer += L"Имя значения: " +  std::wstring(valueName) + L'\n';

                if (dataType == REG_SZ) {
                    LPCWSTR strValue = reinterpret_cast<LPCWSTR>(data);
                    answer += L"Данные значения: " + std::wstring(strValue) + L'\n';
                }
                else if (dataType == REG_DWORD) {
                    DWORD intValue = *reinterpret_cast<DWORD*>(data);
                    answer += L"Данные значения: " + std::wstring(std::to_wstring(intValue)) + L'\n';
                }
                else {
                    answer += L"Другой тип данных (не обработан).\n";
                }

                // Увеличение индекса и сброс размеров
                valueIndex++;
                valueNameSize = 256;
                dataSize = 256;
                answer += L'\n';
            }
            else {
                wprintf(L"Ошибка при перечислении значений: %d\n", result);
                break;
            }
        }
        return answer;
        RegCloseKey(hKey);
    }
    else {
        wprintf(L"Ошибка при открытии ключа: %d\n", result);
    }
    //HKEY hKey = HKEY_CURRENT_USER;
    //std::wstring subkeyName = L"Software\\Lab5";
    //DWORD dataType;
    //BYTE* data = nullptr;
    //DWORD dataSize = 0;

    //if (RegOpenKeyExW(hKey, subkeyName.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    //{
    //    long result = RegQueryValueExW(hKey, (LPCWSTR)name.c_str(), NULL, &dataType, NULL, &dataSize);

    //    if (result == ERROR_SUCCESS) {
    //        // Выделение памяти для данных значения
    //        data = new BYTE[dataSize];

    //        result = RegQueryValueExW(hKey, (LPCWSTR)name.c_str(), NULL, &dataType, data, &dataSize);

    //        if (result == ERROR_SUCCESS) {
    //            if (dataType == REG_SZ) {
    //                // Если тип данных - строка (REG_SZ)
    //                LPCWSTR strValue = reinterpret_cast<LPCWSTR>(data);
    //                return strValue;
    //            }
    //            else if (dataType == REG_DWORD) {
    //                // Если тип данных - 32-битное целое число (REG_DWORD)
    //                DWORD intValue = *reinterpret_cast<DWORD*>(data);
    //                return std::wstring(std::to_wstring(intValue));
    //            }
    //            else {
    //                wprintf(L"Другой тип данных (не обработан).\n");
    //            }
    //        }
    //        else {
    //            wprintf(L"Ошибка при получении значения: %d\n", result);
    //        }
    //    }
    //    else {
    //        wprintf(L"Ошибка при получении информации о значении: %d\n", result);
    //    }
    //}
    //return L"";
}