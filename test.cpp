/*******************************/
/* test.cpp */
/*******************************/
#include <cstdio>
#include "sys.h"
#include "rtos_api.h"

DeclareTask(Task1, 4);

DeclareTask(Task2, 2);

DeclareTask(Task3, 3);

DeclareTask(Task5, 2);

DeclareTask(Task6, 1);

DeclareTask(Task7, 2);

DeclareTask(Task8, 3);

DeclareTask(Task9, 11);

DeclareTask(Task10, 7);

DeclareTask(Task11, 4);

DeclareTask(Task12, 4);
DeclareSysEvent(1);
DeclareSysEvent(2);
DeclareSysEvent(3);

int main() {
    printf("Hello!\n");
    InitPVS(1, "Res1");
    InitPVS(2, "Res2");
    //StartOS(Task1, Task1prior, "Task1");
    //StartOS(Task5, Task5prior, "Task5");
    StartOS(Task7, Task7prior, "Task7");
//    StartOS(Task9, Task9prior, "Task9");
    ShutdownOS();
    return 0;
}
// 4
TASK(Task1) {
    printf("Start Task1\n");
    ActivateTask(Task2, Task2prior, "Task2");
    printf("Task1\n");
    TerminateTask();
}
// 2
TASK(Task2) {
    printf("Start Task2\n");
    ActivateTask(Task3, Task3prior, "Task3");
    Dispatch();
    printf("Task2\n");
    TerminateTask();
}
// 3
TASK(Task3) {
    printf("Start Task3\n");
    Dispatch();
    printf("Task3\n");
    TerminateTask();
}
//-------------------------------------
// 2
TASK(Task5) {
    printf("Start Task5\n");
    P(1);
    ActivateTask(Task6, Task6prior, "Task6");
    V(1);
    printf("Task5\n");
    TerminateTask();
}
// 1
TASK(Task6) {
    printf("Start Task6\n");
    P(1);
    V(1);
    printf("Task6\n");
    TerminateTask();
}
//-------------------------------------
// 11
TASK(Task9) {
    printf("Start Task9\n");
    P(1);
    ActivateTask(Task10, Task10prior, "Task10");
    P(2);
    V(1);
    V(2);
    printf("Task9\n");
    TerminateTask();
}
// 7
TASK(Task10) {
    printf("Start Task10\n");
    P(2);
    P(1);
    V(1);
    V(2);
    printf("Task10\n");\
TerminateTask();
}

//-------------------------------------
// 2
int Task7_ID;

TASK(Task7) {
    printf("Start Task7\n");
    Task7_ID = RunningTask;
    ActivateTask(Task8, Task8prior, "Task8");
    WaitSysEvent(Event_1);
    TEventMask evnt;
    GetSysEvent(&evnt);
    printf("GetEvent=%i\n", evnt);
    printf("Task7\n");
    TerminateTask();
}
// 3
TASK(Task8) {
    printf("Start Task8\n");
    SetSysEvent(Event_1);
    printf("Task8\n");
    TerminateTask();
}