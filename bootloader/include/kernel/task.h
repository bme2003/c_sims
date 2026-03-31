#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H

#include "kernel/types.h"

#define MAX_TASKS 8
#define KERNEL_STACK_SIZE 4096u

enum task_state
{
    TASK_UNUSED,
    TASK_RUNNABLE,
    TASK_RUNNING,
    TASK_BLOCKED
};

struct cpu_context
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t esp;
    uint32_t eip;
    uint32_t eflags;
};

struct task
{
    int pid;
    const char *name;
    enum task_state state;
    uint32_t *kernel_stack_base;
    uint32_t *kernel_stack_top;
    uint32_t run_ticks;
    uint32_t yield_count;
    uint32_t switch_count;
    struct cpu_context context;
};

#endif
