# Unix-Like Kernel Plan

This plan assumes continued development of a custom kernel with a Unix-like design:

- process-based execution
- per-process virtual address spaces
- system calls
- files and file descriptors
- a shell in user space, not inside the kernel

The current repo already has:

- BIOS Stage 1 boot
- Stage 2 loader
- switch to 32-bit protected mode
- a tiny freestanding kernel entry

The next steps are not "write all of Unix at once." Instead, build the minimum layers in the right order.

## Phase 1: Stabilize Boot And Kernel Bring-Up

Goal: make the system reliable enough that kernel work is easy to debug.

Build next:

1. Better text output
2. Panic/assert support
3. Boot info handoff from Stage 2 to the kernel
4. Better disk read error handling
5. A cleaner kernel source layout

Suggested layout:

```text
bootloader/
  stage1.asm
  stage2.asm
  kernel_entry.asm
  kernel.c
kernel/
  arch/x86/
    gdt.c
    idt.c
    isr.asm
    irq.asm
    pic.c
    pit.c
  console/
    vga.c
    printk.c
  mm/
    pmm.c
    paging.c
    kmalloc.c
  sched/
    task.c
    switch.asm
  fs/
    vfs.c
    initrd.c
  user/
    elf.c
    syscall.c
include/
  kernel/
    boot.h
    console.h
    panic.h
    idt.h
    pmm.h
    paging.h
    task.h
    syscall.h
    vfs.h
```

Pseudocode:

```c
// include/kernel/boot.h
struct boot_info {
    uint8_t boot_drive;
    uint32_t kernel_load_addr;
    uint32_t memory_kb;
};
```

```asm
; stage2.asm
; fill a boot_info struct in low memory
boot_info.boot_drive = dl
boot_info.kernel_load_addr = 0x10000
boot_info.memory_kb = bios_detect_memory()

enter_protected_mode()
push boot_info_address
jump kernel_start
```

```asm
; kernel_entry.asm
kernel_start:
    setup_stack()
    read boot_info_ptr from stack/register
    call kernel_main(boot_info_ptr)
    halt_forever()
```

```c
// kernel/main.c
void kernel_main(struct boot_info *boot)
{
    console_init();
    printk("kernel boot ok\n");
    printk("boot drive = %x\n", boot->boot_drive);
    panic_if_bad_state();
    for (;;)
    {
        asm_hlt();
    }
}
```

## Phase 2: Interrupts And Hardware Basics

Goal: stop being a passive kernel and start reacting to the machine.

Build next:

1. GDT cleanup
2. IDT
3. CPU exception handlers
4. PIC remap
5. PIT timer
6. PS/2 keyboard input

Why this matters:

- exceptions make faults visible instead of silently freezing
- timer interrupts are the foundation for scheduling
- keyboard input enables an interactive kernel console

Pseudocode:

```c
// arch/x86/idt.c
void idt_init(void)
{
    set_idt_gate(0, isr0);   // divide by zero
    set_idt_gate(13, isr13); // general protection fault
    set_idt_gate(14, isr14); // page fault
    set_idt_gate(32, irq0);  // PIT
    set_idt_gate(33, irq1);  // keyboard
    lidt(&idt_ptr);
    sti();
}
```

```c
// arch/x86/pit.c
volatile uint64_t ticks = 0;

void pit_handler(void)
{
    ticks++;
    scheduler_tick();
    send_eoi(0);
}
```

```c
// arch/x86/keyboard.c
void keyboard_irq_handler(void)
{
    uint8_t scancode = inb(0x60);
    char c = translate_scancode(scancode);
    if (c != 0)
    {
        console_putc(c);
    }
    send_eoi(1);
}
```

## Phase 3: Memory Management

Goal: make the kernel capable of safely owning memory like a real Unix-like system.

Build next:

1. Physical memory map detection
2. Physical page allocator
3. Paging
4. Kernel heap allocator
5. Separate kernel and user memory later

Why this matters:

- Unix-like processes depend on isolated address spaces
- stable process management depends on paging

Pseudocode:

```c
// mm/pmm.c
void pmm_init(memory_map map)
{
    for each region in map
    {
        if (region.usable)
        {
            mark_pages_free(region.start, region.length);
        }
    }

    reserve_kernel_pages();
    reserve_bootloader_pages();
}
```

```c
// mm/paging.c
void paging_init(void)
{
    page_directory = alloc_page();
    identity_map_low_memory();
    map_kernel_higher_half_later_if_desired();
    load_cr3(page_directory);
    enable_paging_bit();
}
```

```c
// mm/kmalloc.c
void *kmalloc(size_t size)
{
    block = find_free_block(size);
    if (!block)
    {
        block = grow_heap_with_pages(size);
    }
    return split_and_return(block, size);
}
```

## Phase 4: Process Model And Scheduler

Goal: move from "one running kernel flow" to multiple tasks.

Build next:

1. Task struct
2. Kernel threads first
3. Context switching
4. Round-robin scheduler
5. Sleep/wakeup

Do not jump to user mode first. Start with kernel tasks, because they are much easier to debug.

Pseudocode:

```c
// include/kernel/task.h
struct task {
    int pid;
    cpu_context context;
    page_directory_t *address_space;
    enum task_state state;
    struct file *fd_table[MAX_FDS];
};
```

```c
// sched/task.c
task_t *task_create_kernel(void (*entry)(void))
{
    task_t *task = kmalloc(sizeof(task_t));
    task->pid = next_pid++;
    task->stack = alloc_kernel_stack();
    task->context.eip = entry;
    task->state = TASK_RUNNABLE;
    enqueue_runqueue(task);
    return task;
}
```

```c
// sched/task.c
void scheduler_tick(void)
{
    current = pick_next_runnable_task();
    context_switch(previous, current);
}
```

## Phase 5: System Call Interface

Goal: create the boundary between user programs and the kernel.

Build next:

1. syscall entry mechanism
2. syscall table
3. minimal syscalls
4. copy data safely between user and kernel memory

Start tiny. A good first syscall set:

- `write`
- `read`
- `exit`
- `fork` later
- `execve` later
- `open` after VFS exists

Pseudocode:

```c
// user/syscall.c
int syscall_dispatch(int number, uint32_t a, uint32_t b, uint32_t c)
{
    switch (number)
    {
        case SYS_write:
            return sys_write((int)a, (const char *)b, (size_t)c);
        case SYS_exit:
            return sys_exit((int)a);
        default:
            return -ENOSYS;
    }
}
```

```c
// kernel/sys_write.c
int sys_write(int fd, const char *user_buf, size_t len)
{
    if (!user_pointer_valid(user_buf, len))
    {
        return -EFAULT;
    }

    if (fd == STDOUT_FILENO)
    {
        copy user_buf into kernel temp buffer
        console_write(temp, len);
        return len;
    }

    return vfs_write(fd, user_buf, len);
}
```

## Phase 6: User Mode And ELF Loading

Goal: run programs outside the kernel.

Build next:

1. ring 3 support
2. TSS for safe privilege changes
3. ELF parser/loader
4. first user program

This is the point where the system starts to feel much more Unix-like.

Pseudocode:

```c
// user/elf.c
process_t *load_elf_process(const uint8_t *file_data)
{
    parse_elf_header(file_data);
    proc = process_create();

    for each loadable_segment
    {
        map_user_pages(proc, segment.vaddr, segment.memsz);
        copy_segment_bytes(proc, segment.vaddr, segment.data, segment.filesz);
        zero_bss_tail(proc, segment.vaddr, segment.filesz, segment.memsz);
    }

    proc->entry = elf_header.entry;
    return proc;
}
```

```c
// user/exec.c
void enter_user_mode(process_t *proc)
{
    setup_ring3_segments();
    setup_tss_kernel_stack(current_kernel_stack);
    iret_to(proc->entry, proc->user_stack_top);
}
```

## Phase 7: VFS And Filesystem

Goal: make files, directories, and executable loading work like a Unix-style system.

Build next:

1. VFS abstraction
2. initramfs or initrd first
3. simple filesystem second
4. file descriptors per process
5. `/bin`, `/etc`, `/dev` layout later

Do not start with a complicated on-disk filesystem. An initrd is much easier and gets user programs running faster.

Pseudocode:

```c
// fs/vfs.c
int vfs_open(const char *path, int flags)
{
    vnode_t *node = vfs_lookup(path);
    if (!node)
    {
        return -ENOENT;
    }

    file_t *file = file_create(node, flags);
    return install_fd(current_task, file);
}
```

```c
// fs/initrd.c
void initrd_mount(void *archive_start)
{
    for each archive_entry
    {
        vnode = create_vnode(entry.name, entry.type);
        vnode->data = entry.data;
        vnode->size = entry.size;
        attach_to_tree(vnode);
    }
}
```

## Phase 8: Unix-Like Process Behavior

Goal: start behaving like a recognizable Unix-style system.

Build next:

1. `fork`
2. `execve`
3. `waitpid`
4. stdin/stdout/stderr
5. pipes
6. signals later

Pseudocode:

```c
// sched/fork.c
int sys_fork(void)
{
    child = duplicate_current_task();
    child->address_space = clone_address_space(current->address_space);
    child->context.eax = 0;
    enqueue_runqueue(child);
    return child->pid;
}
```

```c
// user/execve.c
int sys_execve(const char *path, char *const argv[])
{
    file = vfs_read_file(path);
    proc = load_elf_into_existing_process(current, file);
    replace_current_image(proc);
    jump_to_user_entry();
}
```

## Phase 9: Your Shell In User Space

Goal: make the shell a normal program, not a kernel feature.

This is where the current shell project becomes very useful. Reuse its parser and command ideas, but redesign it around system calls:

- `fork`
- `execve`
- `waitpid`
- `pipe`
- `dup2`
- `chdir`

Pseudocode:

```c
// userspace shell
while (true)
{
    print_prompt();
    line = read_line();
    cmd = parse(line);

    if (is_builtin(cmd))
    {
        run_builtin(cmd);
        continue;
    }

    pid = fork();
    if (pid == 0)
    {
        execve(cmd.path, cmd.argv);
        write(2, "exec failed\n", 12);
        exit(1);
    }
    else
    {
        waitpid(pid, &status, 0);
    }
}
```

## Recommended Immediate Next 5 Tasks For This Repo

For the best order from the current repo state, do these next:

1. Pass a `boot_info` struct from Stage 2 to the kernel
2. Split `kernel.c` into `kernel/main.c`, `console/vga.c`, and `console/printk.c`
3. Add IDT setup and exception handlers
4. Add PIC remap and PIT timer interrupts
5. Add a simple physical memory manager

## What To Avoid Right Now

Avoid these until the lower layers exist:

- writing a full filesystem first
- writing a shell inside kernel space
- trying to mimic all of Linux syscalls
- trying networking before interrupts and memory are stable
- trying graphics before user mode and files work

## Success Milestones

Use these checkpoints:

1. Kernel prints logs and survives faults with a readable panic screen
2. Timer interrupts fire reliably
3. Keyboard input reaches the kernel
4. Paging turns on without crashing
5. Two kernel tasks can switch back and forth
6. First user ELF runs in ring 3
7. `write(1, "hello", 5)` works from user mode
8. `/bin/sh` launches as a user process

At that point, the project is no longer just a boot demo. It has the beginnings of a real Unix-like operating system.
