#include "SystemInfo.h"
#include "Windows.h"
#include "TCHAR.h"

SystemInfo::SystemInfo() { }


static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
    static unsigned long long _previousTotalTicks = 0;
    static unsigned long long _previousIdleTicks = 0;

    unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

    float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

    _previousTotalTicks = totalTicks;
    _previousIdleTicks = idleTicks;
    return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME& ft) { return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); }

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
// You'll need to call this at regular intervals, since it measures the load between
// the previous call and the current one.  Returns -1.0 on error.
float GetCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
}

void SystemInfo::GetWorkloadInfo(double &value)
{
    //// Получение информации о загруженности процессора
    //FILETIME idleTime, kernelTime, userTime;
    //if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
    //    ULONGLONG idleTime64 = ((ULONGLONG)idleTime.dwHighDateTime << 32) + idleTime.dwLowDateTime;
    //    ULONGLONG kernelTime64 = ((ULONGLONG)kernelTime.dwHighDateTime << 32) + kernelTime.dwLowDateTime;
    //    ULONGLONG userTime64 = ((ULONGLONG)userTime.dwHighDateTime << 32) + userTime.dwLowDateTime;

    //    value = 100.0 - (idleTime64 * 100.0 / (kernelTime64 + userTime64));
    //}
    value = GetCPULoad() * 100.0;
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