#include <cstdio>
#include "sys.h"
#include "rtos_api.h"

DeclareTask(Task1, 3);
DeclareTask(Task2, 1);
DeclareTask(Task3, 2);

DeclareTask(Task4, 2);
DeclareTask(Task5, 1);

DeclareTask(Task6, 1);
DeclareTask(Task7, 2);

DeclareTask(Task8, 3);

DeclareTask(Task9, 2);
DeclareTask(Task10, 1);

DeclareSysEvent(1);
DeclareSysEvent(2);

int main() {
    printf("Hello!\n");
    InitPVS(1, "Res1");
    InitPVS(2, "Res2");
    //StartOS(Task1, Task1prior, "Task1");
    //StartOS(Task4, Task4prior, "Task4");
    //StartOS(Task6, Task6prior, "Task6");
    //StartOS(Task8, Task8prior, "Task8");
    StartOS(Task9, Task8prior, "Task9");
    ShutdownOS();
    return 0;
}
//1st Example
//Prior 3
TASK(Task1) {
    printf("Start Task1\n");
    ActivateTask(Task2, Task2prior, "Task2");
    printf("Task1 Completed\n");
    TerminateTask();
}
//Prior 2
TASK(Task2) {
    printf("Start Task2\n");
    ActivateTask(Task3, Task3prior, "Task3");
    Dispatch();
    printf("Task2 Completed\n");
    TerminateTask();
}
//Prior 1
TASK(Task3) {
    printf("Start Task3\n");
    Dispatch();
    printf("Task3 Completed\n");
    TerminateTask();
}

//2nd Example
//Prior 2
TASK(Task4) {
    printf("Start Task5\n");
    GetResource(1);
    ActivateTask(Task5, Task5prior, "Task5");
    ReleaseResource(1);
    printf("Task4 Completed\n");
    TerminateTask();
}
//Prior 1
TASK(Task5) {
    printf("Start Task5\n");
    GetResource(1);
    printf("Task5 Completed\n");
    ReleaseResource(1);
    TerminateTask();
}

//3rd Example
//Prior 1
int taskId;
TASK(Task6) {
    printf("Start Task6\n");
    taskId = RunningTask;
    ActivateTask(Task7, Task7prior, "Task7");
    WaitSysEvent(Event_1);
    TEventMask event;
    GetSysEvent(&event);
    printf("GetEvent = %i\n", event);
    printf("Task6 Completed\n");
    TerminateTask();
}
//Prior 2
TASK(Task7) {
    printf("Start Task7\n");
    SetSysEvent(Event_1);
    printf("Task7 Completed\n");
    TerminateTask();
}

//4th Example
//prior 3
TASK(Task8) {
    printf("Start Task8\n");
    taskId = RunningTask;
    ActivateTask(Task7, Task7prior, "Task7");
    WaitSysEvent(Event_1);
    TEventMask event;
    GetSysEvent(&event);
    printf("GetEvent = %i\n", event);
    printf("Task8 Completed\n");
    TerminateTask();
}

//5th Example
//Prior 2
TASK(Task9) {
    printf("Start Task9\n");
    GetResource(1);
    ActivateTask(Task10, Task10prior, "Task10");
    printf("Task9 Completed\n");
    TerminateTask();
}
//Prior 1
TASK(Task10) {
    printf("Start Task10\n");
    GetResource(1);
    printf("Task10 Completed\n");
    TerminateTask();
}