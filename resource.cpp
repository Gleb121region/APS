#include "sys.h"
#include "rtos_api.h"
#include <algorithm>

void InitPVS(TSemaphore S, std::string name) {
    printf("InitPVS %s\n", name.c_str());
    ResourceQueue[S].name = name;
    ResourceQueue[S].block = 0;
}

void GetResource(TSemaphore S) {
    printf("GetResource %s\n", ResourceQueue[S].name.c_str());
    while (ResourceQueue[S].block) { // ресурс заблокирован
        printf("Resource is blocked\n");
        TaskQueue[RunningTask].task_state = TASK_WAITING;
        TaskQueue[RunningTask].waited_resource = S;
        Dispatch();
    }
    ResourceQueue[S].block = 1;
    printf("End of GetResource %s\n", ResourceQueue[S].name.c_str());
}

void ReleaseResource(TSemaphore S) {
    printf("ReleaseResource %s\n", ResourceQueue[S].name.c_str());
    ResourceQueue[S].block = 0;
    for (auto &task : TaskQueue) {
        if (task.task_state == TASK_WAITING && task.waited_resource == S) {
            task.task_state = TASK_READY;
            task.waited_resource = -1;
        }
    }
    Dispatch();
    printf("End of ReleaseResource %s\n", ResourceQueue[S].name.c_str());
}
