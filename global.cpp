#include "sys.h"

TTask TaskQueue[MAX_TASK];
TResource ResourceQueue[MAX_RES];
int RunningTask;
int HeadTasks[MAX_PRIORITY];
int TaskCount;
int FreeTask;
jmp_buf MainContext;
TEventMask WorkingEvents;