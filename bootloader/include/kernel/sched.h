#ifndef KERNEL_SCHED_H
#define KERNEL_SCHED_H

#include "kernel/task.h"

void task_init(void);
struct task *task_create_kernel(void (*entry)(void));
void scheduler_start(void);
void scheduler_timer_tick(void);
int scheduler_should_yield(void);
void scheduler_yield(void);
struct task *scheduler_current_task(void);
uint32_t scheduler_task_count(void);

#endif
