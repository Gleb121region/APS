#pragma once

#include "defs.h"
#include <csetjmp>
#include <string>

enum TTaskState {
    TASK_RUNNING,
    TASK_READY,
    TASK_SUSPENDED,
    TASK_WAITING
};
typedef unsigned int TEventMask;
typedef int TSemaphore;
typedef struct Type_Task {
    int next, prev;
    int priority;
    int ceiling_priority;

    void (*entry)(void);

    const char *name;
    TTaskState task_state;
    int switch_count;
    jmp_buf context;
    TSemaphore waited_resource;
    TEventMask waiting_events;
} TTask;
typedef struct Type_resource {
    volatile int block;
//int priority;
    std::string name;
} TResource;
extern TTask TaskQueue[MAX_TASK];
extern TResource ResourceQueue[MAX_RES];
//текущая выполняющаяся задача, изменять только в функции TaskSwitch
extern int RunningTask;
extern int HeadTasks[MAX_PRIORITY]; //первая задача в списке
extern int TaskCount;
extern int FreeTask;
extern TEventMask WorkingEvents;//сработавшие события
extern jmp_buf MainContext; //главный контекст
void Schedule(int task);

void Dispatch();