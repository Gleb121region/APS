#pragma once

#include "sys.h"
#include <string>

#define DeclareTask(TaskID, priority)\
TASK(TaskID); \
enum {TaskID##prior=(priority)}
#define TASK(TaskID) void TaskID(void)

typedef void TTaskCall(void);

int ActivateTask(TTaskCall entry, int priority, const char *name);

void TerminateTask(void);

int StartOS(TTaskCall entry, int priority, const char *name);

void ShutdownOS();

void InitPVS(TSemaphore S, std::string name);

void GetResource(TSemaphore S);

void ReleaseResource(TSemaphore S);

#define DeclareSysEvent(ID) \
const int Event_##ID = (ID)*(ID);

void SetSysEvent(TEventMask mask);

void GetSysEvent(TEventMask *mask);

void WaitSysEvent(TEventMask mask);