/*********************************/
/* event.cpp */
/*********************************/
#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

void SetSysEvent(TEventMask mask) {
    printf("SetSysEvent %i\n", mask);
    WorkingEvents |= mask;
    for (int i = 0; i < MAX_TASK; i++) {
        TTask* task = &TaskQueue[i];
        bool taskWaiting = task->task_state == TASK_WAITING;
        bool taskEvents = (WorkingEvents & task->waiting_events);
        if (taskWaiting && taskEvents) {
            task->waiting_events &= ~mask;
            task->task_state = TASK_READY;

            printf("Task \"%s\" is ready\n", task->name);
        }
    }
    WorkingEvents &= ~mask;
    Dispatch();
    printf("End of SetSysEvent %i\n", mask);
}

void GetSysEvent(TEventMask *mask) {
    *mask = WorkingEvents;
}

void WaitSysEvent(TEventMask mask) {
    TTask* currentTask = &TaskQueue[RunningTask];
    currentTask->waiting_events = mask;
    printf("WaitSysEvent %i\n", mask);
    if ((WorkingEvents & mask) == 0) {
        currentTask->task_state = TASK_WAITING;
        Dispatch();
    }
    printf("End of WaitSysEvent %i\n", mask);
}
