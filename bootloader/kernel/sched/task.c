#include "kernel/pmm.h"
#include "kernel/panic.h"
#include "kernel/sched.h"

extern void context_enter_first_task(struct cpu_context *new_context);
extern void context_switch(struct cpu_context *old_context, struct cpu_context *new_context);

static struct task task_table[MAX_TASKS];
static struct task *current_task = (struct task *)0;
static struct task *idle_task = (struct task *)0;
static uint32_t live_task_count = 0;
static uint32_t scheduler_ticks = 0;
static volatile int reschedule_requested = 0;
static int next_pid = 1;

/* Clear one saved CPU context before it is attached to a fresh task. */
static void clear_context(struct cpu_context *context)
{
    context->edi = 0;
    context->esi = 0;
    context->ebp = 0;
    context->ebx = 0;
    context->edx = 0;
    context->ecx = 0;
    context->eax = 0;
    context->esp = 0;
    context->eip = 0;
    context->eflags = 0;
}

/* Reset one task slot back to a clean, unused state. */
static void clear_task(struct task *task)
{
    task->pid = 0;
    task->name = (const char *)0;
    task->state = TASK_UNUSED;
    task->kernel_stack_base = (uint32_t *)0;
    task->kernel_stack_top = (uint32_t *)0;
    clear_context(&task->context);
}

/* Find the next free slot in the fixed-size early task table. */
static struct task *find_free_task_slot(void)
{
    uint32_t index = 0;

    for (index = 0; index < MAX_TASKS; index++)
    {
        if (task_table[index].state == TASK_UNUSED)
        {
            return &task_table[index];
        }
    }

    return (struct task *)0;
}

/* Pick the next runnable task after the current one using round-robin order. */
static struct task *pick_next_runnable_task(void)
{
    uint32_t start_index = 0;
    uint32_t offset = 0;

    if (current_task != (struct task *)0)
    {
        start_index = (uint32_t)(current_task - &task_table[0]) + 1u;
    }

    for (offset = 0; offset < MAX_TASKS; offset++)
    {
        uint32_t index = (start_index + offset) % MAX_TASKS;

        if (task_table[index].state == TASK_RUNNABLE)
        {
            return &task_table[index];
        }
    }

    if (idle_task != (struct task *)0 && idle_task->state == TASK_RUNNABLE)
    {
        return idle_task;
    }

    return current_task;
}

/* Initialize the fixed-size task table before any kernel tasks are created. */
void task_init(void)
{
    uint32_t index = 0;

    for (index = 0; index < MAX_TASKS; index++)
    {
        clear_task(&task_table[index]);
    }

    current_task = (struct task *)0;
    idle_task = (struct task *)0;
    live_task_count = 0;
    scheduler_ticks = 0;
    reschedule_requested = 0;
    next_pid = 1;
}

/* Create one named kernel task with its own stack and an initial saved context. */
struct task *task_create_kernel_named(const char *name, void (*entry)(void))
{
    struct task *task = find_free_task_slot();
    void *stack_page = (void *)0;

    if (task == (struct task *)0 || entry == (void (*)(void))0)
    {
        return (struct task *)0;
    }

    stack_page = pmm_alloc_page();
    if (stack_page == (void *)0)
    {
        return (struct task *)0;
    }

    clear_task(task);
    task->pid = next_pid++;
    task->name = name;
    task->state = TASK_RUNNABLE;
    task->kernel_stack_base = (uint32_t *)stack_page;
    task->kernel_stack_top = (uint32_t *)((uint32_t)stack_page + KERNEL_STACK_SIZE);
    task->context.esp = (uint32_t)task->kernel_stack_top;
    task->context.eip = (uint32_t)entry;
    task->context.eflags = 0x00000202u;
    live_task_count++;

    return task;
}

/* Create one unnamed kernel task when no label is provided yet. */
struct task *task_create_kernel(void (*entry)(void))
{
    return task_create_kernel_named("unnamed", entry);
}

/* Report which task is currently active once real switching is added later. */
struct task *scheduler_current_task(void)
{
    return current_task;
}

/* Report the current task name so logs can describe scheduler state. */
const char *scheduler_current_task_name(void)
{
    if (current_task == (struct task *)0 || current_task->name == (const char *)0)
    {
        return "none";
    }

    return current_task->name;
}

/* Start the first runnable task by loading its saved context in assembly. */
void scheduler_start(void)
{
    current_task = pick_next_runnable_task();
    if (current_task != (struct task *)0)
    {
        current_task->state = TASK_RUNNING;
        context_enter_first_task(&current_task->context);
    }

    panic("scheduler_start found no runnable task");
}

/* Mark that the current task should yield once a timer quantum expires. */
void scheduler_timer_tick(void)
{
    scheduler_ticks++;

    if (current_task != (struct task *)0 && (scheduler_ticks % 4u) == 0)
    {
        reschedule_requested = 1;
    }
}

/* Let task code observe and consume one pending reschedule request. */
int scheduler_should_yield(void)
{
    int should_yield = reschedule_requested;

    reschedule_requested = 0;
    return should_yield;
}

/* Cooperatively switch to the next runnable task from the current task. */
void scheduler_yield(void)
{
    struct task *previous = current_task;
    struct task *next = pick_next_runnable_task();

    if (previous == (struct task *)0 || next == (struct task *)0 || previous == next)
    {
        return;
    }

    if (previous->state == TASK_RUNNING)
    {
        previous->state = TASK_RUNNABLE;
    }

    next->state = TASK_RUNNING;
    current_task = next;
    context_switch(&previous->context, &next->context);
}

/* Report how many task slots are currently populated and runnable/active. */
uint32_t scheduler_task_count(void)
{
    return live_task_count;
}

/* Register one task as the scheduler fallback when nothing else can run. */
void scheduler_set_idle_task(struct task *task)
{
    idle_task = task;
}
