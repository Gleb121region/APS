#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

// добавить задачу перед item, head указывает на голову списка
void InsertTaskBefore(int task, int item, int *head) {
    if (TaskQueue[task].next != -1)
        return;
    if (item == *head)
        *head = task;
    TaskQueue[task].next = item;
    TaskQueue[task].prev = TaskQueue[item].prev;
    TaskQueue[TaskQueue[item].prev].next = task;
    TaskQueue[item].prev = task;
}

void InsertTaskAfter(int task, int item) {
    if (TaskQueue[task].next != -1)
        return;
    TaskQueue[task].next = TaskQueue[item].next;
    TaskQueue[task].prev = item;
    TaskQueue[TaskQueue[item].next].prev = task;
    TaskQueue[item].next = task;
}

void RemoveTask(int task, int* head) {
    if (TaskQueue[task].next == -1)
        return;
    int prev = TaskQueue[task].prev;
    int next = TaskQueue[task].next;
    TaskQueue[prev].next = next;
    TaskQueue[next].prev = prev;
    TaskQueue[task].next = -1;
    TaskQueue[task].prev = -1;
    if (*head == task) {
        *head = next;
        if (next == task) {
            *head = -1;
        }
    }
}

int ActivateTask(TTaskCall entry, int priority, const char *name) {
    int task, occupy;
    printf("ActivateTask %s\n", name);
    task = HeadTasks[priority];
    occupy = FreeTask;
//изменяем список свободных задач
    RemoveTask(occupy, &FreeTask);
    TaskQueue[occupy].priority = priority;
    TaskQueue[occupy].ceiling_priority = priority;
    TaskQueue[occupy].name = name;
    TaskQueue[occupy].entry = entry;
    TaskQueue[occupy].switch_count = 0;
    TaskQueue[occupy].task_state = TASK_READY;
    TaskCount++;
    printf("End of ActivateTask %s\n", name);
    Schedule(occupy);
    if (task != HeadTasks[priority]) {
        Dispatch();
    }
    return occupy;
}

void TerminateTask(void) {
    TaskCount--;
    int task = RunningTask;
    printf("TerminateTask %s\n", TaskQueue[task].name);
    TaskQueue[task].task_state = TASK_SUSPENDED;
    RemoveTask(task, &(HeadTasks[TaskQueue[task].ceiling_priority]));
//добавляем задачу в список свободных
    InsertTaskBefore(task, FreeTask, &FreeTask);
    printf("End of TerminateTask %s\n", TaskQueue[task].name);
//задач не осталось, возвращаемся
    if (TaskCount == 0)
        longjmp(MainContext, 1);
    Dispatch();
}

void Schedule(int task) {
    if (TaskQueue[task].task_state == TASK_SUSPENDED)
        return;
    printf("Schedule %s\n", TaskQueue[task].name);
    int priority = TaskQueue[task].ceiling_priority;
    RemoveTask(task, &(HeadTasks[priority]));
    if (HeadTasks[priority] == -1) {
        HeadTasks[priority] = task;
        TaskQueue[task].next = task;
        TaskQueue[task].prev = task;
    }
    else {
        InsertTaskAfter(task, TaskQueue[HeadTasks[priority]].prev);
    }
    printf("End of Schedule %s\n", TaskQueue[task].name);
}

void TaskSwitch(int nextTask) {
    if (nextTask == -1)
        return;
    TTask* previousTask = RunningTask == -1 ? NULL : &TaskQueue[RunningTask];
    TTask* next = &TaskQueue[nextTask];
    if (previousTask != NULL && previousTask->task_state == TASK_RUNNING)
        previousTask->task_state = TASK_READY;
    next->task_state = TASK_RUNNING;
    RunningTask = nextTask;
    next->switch_count++;
    if (next->switch_count == 1) {
        printf("Dispatch - run task %s\n", next->name);
        next->entry();
    }
    else {
        printf("Dispatch - task switch to %s\n", next->name);
        longjmp(next->context, 1);
    }
}

// резервируем область стека для задачи
void TaskSwitchWithCushion(int nextTask) {
    char space[9000];
    space[8999] = 1; // отключаем оптимизацию массивов
    TaskSwitch(nextTask);
}

void Dispatch() {
    if (RunningTask != -1)
        printf("Dispatch - %s\n", TaskQueue[RunningTask].name);
    else
        printf("Dispatch\n");

    int nextTask = -1; // Инициализируем следующую задачу значением -1

    // Ищем следующую готовую задачу в порядке приоритетов
    for (int i = 0; i < MAX_PRIORITY; i++) {
        if (HeadTasks[i] != -1) { // Если есть задачи с текущим приоритетом
            int currentTask = HeadTasks[i]; // Берем первую задачу в списке задач с текущим приоритетом
            do {
                if (TaskQueue[currentTask].task_state == TASK_READY || TaskQueue[currentTask].task_state == TASK_RUNNING) {
                    nextTask = currentTask; // Задача найдена, записываем ее номер в nextTask
                    break;
                }
                currentTask = TaskQueue[currentTask].next; // Переходим к следующей задаче в списке с текущим приоритетом
            } while (currentTask != HeadTasks[i]); // Продолжаем искать, пока не пройдем все задачи с текущим приоритетом

            if (nextTask != -1) // Если задача найдена, выходим из цикла
                break;
        }
    }

    if (nextTask != -1) { // Если следующая задача найдена
        if (RunningTask == -1 || TaskQueue[RunningTask].task_state == TASK_SUSPENDED) // Если текущая задача не запущена или приостановлена
            TaskSwitch(nextTask);
        else if (RunningTask != nextTask) { // Если текущая задача не является следующей задачей
            if (!setjmp(TaskQueue[RunningTask].context)) { // Сохраняем контекст текущей задачи перед переключением
                if (TaskQueue[RunningTask].switch_count == 1) // Если текущая задача была запущена первый раз, используем функцию TaskSwitchWithCushion
                    TaskSwitchWithCushion(nextTask);
                else // Иначе используем обычную функцию TaskSwitch
                    TaskSwitch(nextTask);
            }
        }
    }
    else { // Если следующая задача не найдена
        printf("Error - don't have ready tasks\n");
        longjmp(MainContext, 1);
    }

    // Перемещаем выполненную задачу в конец списка задач с текущим приоритетом
    RemoveTask(nextTask, &(HeadTasks[TaskQueue[nextTask].priority]));
    if (HeadTasks[TaskQueue[nextTask].priority] == -1) { // Если в списке задач с текущим приоритетом нет задач, вставляем задачу с номером nextTask
        HeadTasks[TaskQueue[nextTask].priority] = TaskQueue[nextTask].next = TaskQueue[nextTask].prev = nextTask;
    }
    else { // Иначе вставляем задачу после последней задачи в списке
        InsertTaskAfter(nextTask, TaskQueue[HeadTasks[TaskQueue[nextTask].priority]].prev);
    }
    printf("End of Dispatch - %s\n", TaskQueue[RunningTask].name);
}
