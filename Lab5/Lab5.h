#pragma once

#include "resource.h"
#include <windows.h>
#include <string>

#define CREATE_KEY 1101
#define DELETE_KEY 1102
#define FIND_KEY 1103
#define DELETE_VALUE 1104

HWND hEditCreateKeyName;
HWND hEditCreateKeyValue;
HWND hEditCreateKeyValueData;
HWND hButtonCreateKey;
HWND hEditDeleteKeyName;
HWND hComboBoxType;
HWND hButtonDeleteKey;
HWND hEditFindKeyName;
HWND hButtonDeleteValue;
HWND hEditDeleteValueName;
HWND hButtonFindKey;
bool CreateKey(std::wstring keyName, std::wstring name, std::wstring value);
bool DeleteKey(std::wstring name);
bool DeleteValue(std::wstring name);
std::wstring FindKey(std::wstring name);