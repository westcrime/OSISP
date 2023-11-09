#pragma once

#include "resource.h"
#include <string>
#include <Windows.h>
#include <iostream>

#define START_DINNER 1101

HWND hEditPhilosophersNumber;
HWND hEditLog;
HWND hButtonStartDinner;

int NUM_PHILOSOPHERS;
HANDLE editMutex;
HANDLE* forks;
HANDLE monitorMutex;

void PrintMessage(std::wstring message);
DWORD WINAPI philosopherThread(LPVOID param);
DWORD WINAPI startDinner(LPVOID param);

#define THINKING 1103
#define HUNGRY 1102
#define EATING 1104
int times = 1;

std::string my_log;

class Monitor {
private:
    int *state;
    CONDITION_VARIABLE *phcond;
    CRITICAL_SECTION condLock;

public:
    Monitor();

    ~Monitor();

    void test(int phnum);

    void take_fork(int phnum);

    void put_fork(int phnum);
};

Monitor *phil_object;
