#include "SystemInfo.h"
#include "Windows.h"
#include "TCHAR.h"

SystemInfo::SystemInfo() { }

void SystemInfo::GetWorkloadInfo(double &value)
{
    // Получение информации о загруженности процессора
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULONGLONG idleTime64 = ((ULONGLONG)idleTime.dwHighDateTime << 32) + idleTime.dwLowDateTime;
        ULONGLONG kernelTime64 = ((ULONGLONG)kernelTime.dwHighDateTime << 32) + kernelTime.dwLowDateTime;
        ULONGLONG userTime64 = ((ULONGLONG)userTime.dwHighDateTime << 32) + userTime.dwLowDateTime;

        value = 100.0 - (idleTime64 * 100.0 / (kernelTime64 + userTime64));
    }
}

void SystemInfo::GetMemoryInfo(double& value)
{
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    if (GlobalMemoryStatusEx(&memoryStatus)) {
        DWORDLONG totalPhysicalMemory = memoryStatus.ullTotalPhys;
        DWORDLONG freePhysicalMemory = memoryStatus.ullAvailPhys;
        value = 100.0 - (freePhysicalMemory * 100.0 / totalPhysicalMemory);
    }
}