# Bootloader And Kernel Guide

This file is a human-oriented guide to the current bootloader and kernel codebase.

The goal is to explain:

- what each file does
- what order the machine runs things in
- what important acronyms mean
- what the hex constants are
- why each subsystem exists
- what assumptions the current code makes

This guide is meant to stay in sync with the code as the project grows.

## Big Picture

The current system is a very early x86 operating system prototype.

What it can currently do:

- boot from a BIOS-style floppy image in QEMU
- load a two-stage bootloader
- switch the CPU from 16-bit real mode to 32-bit protected mode
- enter a freestanding C kernel
- print text directly to the VGA text buffer
- install an IDT
- handle a few CPU exceptions
- handle timer and keyboard interrupts
- show a timer heartbeat
- allocate physical pages
- perform simple kernel allocations
- enable paging
- create kernel task records
- enter the first created kernel task
- cooperatively switch between kernel tasks
- use PIT ticks to request cooperative rescheduling

What it does not do yet:

- real multitasking
- timer-driven preemptive task switching
- user mode
- system calls
- filesystems
- processes in the Unix sense

So the current codebase is not "a full operating system" yet. It is an early kernel bring-up codebase with the first layers of memory, interrupts, and scheduler groundwork.

## The Boot Sequence

The boot path is:

1. BIOS loads the first 512 bytes from disk into memory at `0x7C00`
2. `stage1.asm` runs in 16-bit real mode
3. Stage 1 loads Stage 2 from later disk sectors
4. `stage2.asm` runs in 16-bit real mode
5. Stage 2 loads the kernel binary into memory at `0x10000`
6. Stage 2 enables A20
7. Stage 2 loads a small GDT
8. Stage 2 sets the protected-mode bit in `cr0`
9. Stage 2 jumps into 32-bit code
10. `kernel_entry.asm` sets up a kernel stack and calls `kernel_main`
11. `kernel/main.c` initializes the kernel subsystems
12. The kernel eventually enters the first runnable kernel task
13. Kernel tasks can cooperatively yield to one another

That is the current end-to-end machine path.

## Acronym Glossary

These acronyms appear throughout the codebase:

- `BIOS`: Basic Input/Output System
  Old PC firmware interface used before the kernel takes over the machine.

- `VGA`: Video Graphics Array
  The old PC text/graphics hardware interface. In this project, VGA text mode is used for screen output.

- `GDT`: Global Descriptor Table
  A CPU structure used in x86 protected mode to describe memory segments.

- `IDT`: Interrupt Descriptor Table
  A CPU table that tells the processor which handler to run for each interrupt or exception.

- `ISR`: Interrupt Service Routine
  Generic term for a low-level assembly interrupt/exception entry routine.

- `IRQ`: Interrupt Request
  Hardware interrupt lines, such as the timer or keyboard interrupt.

- `PIC`: Programmable Interrupt Controller
  Old x86 hardware that routes hardware IRQs to the CPU.

- `PIT`: Programmable Interval Timer
  Old x86 timer hardware that generates periodic interrupts.

- `PS/2`: Keyboard/controller interface commonly used by old PC keyboards.

- `PMM`: Physical Memory Manager
  Tracks physical memory pages and hands them out to kernel subsystems.

- `kmalloc`: Kernel malloc
  Dynamic memory allocation inside the kernel.

- `TLB`: Translation Lookaside Buffer
  CPU cache for virtual-to-physical address translations.

- `A20`: Address line 20
  Historically disabled on very old PCs for compatibility. It must be enabled to access memory above 1 MiB correctly.

- `QEMU`: Quick Emulator
  The emulator used to run and test the OS image.

## Important CPU Modes

### Real Mode

The CPU starts in 16-bit real mode when BIOS hands control to the bootloader.

Properties:

- simple addressing
- BIOS interrupts available
- no protection
- no paging
- 1 MiB historical memory model

Both `stage1.asm` and the early part of `stage2.asm` run in real mode.

### Protected Mode

After Stage 2 loads the GDT and sets the PE bit in `cr0`, the CPU enters 32-bit protected mode.

Properties:

- 32-bit registers and instructions
- segmentation enabled via the GDT
- no BIOS services anymore in normal practice
- paging can be enabled later

The kernel runs in protected mode.

### Paging Enabled Protected Mode

Later in kernel initialization, paging is enabled.

This adds:

- virtual memory translation
- page tables and a page directory
- the ability to create non-identity mappings

The current kernel still uses a simple identity-mapped layout, but paging is now active.

## Memory Addresses That Matter

Low-level systems code uses many fixed addresses. These are not random, even if they look intimidating.

### `0x7C00`

This is where BIOS traditionally loads the first boot sector.

Used in:

- `stage1.asm`

Meaning:

- the first bootloader stage begins executing here

### `0x8000`

This is where Stage 2 is loaded.

Used in:

- `stage1.asm`
- `stage2.asm`

Meaning:

- a convenient low-memory location that does not overlap the boot sector

### `0x10000`

This is where the kernel binary is loaded in physical memory.

Used in:

- `stage2.asm`
- `linker.ld`
- boot info handed to the kernel

Meaning:

- the kernel is linked and loaded to start at physical address `64 KiB`

### `0xB8000`

This is the VGA text-mode buffer.

Used in:

- `kernel/console/vga.c`

Meaning:

- writing characters here updates the visible screen in VGA text mode

### `0x9FC00`

This is used as a temporary protected-mode stack in Stage 2.

Meaning:

- it is near the top of conventional memory and below typical reserved video/ROM areas

### `0x00400000`

This is the virtual address used in the paging mapping smoke test.

Meaning:

- a higher virtual address chosen to prove the kernel can make a non-identity mapping


- `0x20`, `0x21`, `0xA0`, `0xA1`
  These are PIC I/O ports.

- `0x40`, `0x43`
  These are PIT I/O ports.

- `0x80000000`
  This is the paging enable bit in `cr0`.

- `0xFFFFF000`
  This is a mask for extracting a page-aligned address from a page-table entry.

- `0x0F`
  This is a VGA text color value.

These numbers are hardware protocol constants, not arbitrary style choices.

## File-By-File Guide

### `bootloader/stage1.asm`

This is the first-stage bootloader.

What it does:

- starts in 16-bit real mode
- sets up `ds`, `es`, `ss`, and `sp`
- stores the BIOS boot drive number
- prints a status message using BIOS video interrupt `0x10`
- loads Stage 2 from disk using BIOS disk interrupt `0x13`
- jumps to the loaded Stage 2 code

Why it is tiny:

- BIOS only loads one sector here
- one sector is `512` bytes total
- it must also end with the boot signature `0xAA55`

Important constants:

- `STAGE2_LOAD_SEGMENT = 0x0800`
  Stage 2 is loaded at physical address `0x8000`

- `STAGE2_SECTORS = 4`
  Stage 1 assumes Stage 2 fits in 4 sectors

### `bootloader/stage2.asm`

This is the second-stage bootloader.

What it does:

- runs in 16-bit real mode at `0x8000`
- records the BIOS boot drive
- asks BIOS for conventional memory size using interrupt `0x12`
- prints a status message
- loads the kernel binary into memory
- enables A20
- installs a tiny GDT
- switches into protected mode
- loads protected-mode segment registers
- passes a boot info pointer in `esi`
- jumps to the kernel entry address

Why Stage 2 exists:

- Stage 1 is too small to do everything
- Stage 2 gives more room for disk loading and protected-mode setup

Important constants:

- `KERNEL_LOAD_ADDRESS = 0x10000`
  Where the kernel is placed in memory

- `KERNEL_START_SECTOR = 6`
  Where the kernel begins in the floppy image

- `KERNEL_SECTORS`
  Injected from the Makefile so it tracks the built kernel size

### `bootloader/kernel_entry.asm`

This is the very first 32-bit kernel entry stub.

What it does:

- sets `esp` to a known kernel stack
- pushes the boot info pointer
- calls `kernel_main`
- halts forever if `kernel_main` returns

Why it exists:

- C code expects a sane stack and calling convention
- Stage 2 is still low-level transition code

### `bootloader/linker.ld`

This is the linker script.

What it does:

- sets the kernel start address to `0x10000`
- places `.text`, `.rodata`, `.data`, and `.bss` in order
- exports `__kernel_start` and `__kernel_end`

Why it matters:

- without this, the linker could place code at addresses that do not match where the bootloader loads it

### `bootloader/Makefile`

This builds the bootloader and kernel image.

What it does:

- picks the cross-compiler tools
- assembles Stage 1 and Stage 2
- assembles the kernel entry and low-level assembly files
- compiles C kernel files
- links the kernel ELF
- converts the ELF to a flat binary
- computes the kernel sector count for Stage 2 automatically
- builds a floppy image with `dd`
- runs the image in QEMU

Why the auto sector count matters:

- when the kernel grows, Stage 2 must load more sectors
- this used to be a source of blank-screen failures

## Kernel Bring-Up Order In `kernel/main.c`

The kernel currently initializes subsystems in this order:

1. console setup
2. boot info printing
3. IDT
4. PIC remap
5. PIT
6. keyboard
7. interrupt enable
8. PMM
9. kmalloc
10. paging
11. paging smoke test
12. task table creation
13. task creation smoke test
14. start the first runnable task

This order is deliberate.

Why it makes sense:

- screen output comes first so failures are visible
- interrupts come before interactive testing
- memory managers come before task stacks
- paging is enabled before more advanced scheduler work

## Interrupt Path

The current interrupt path is:

1. hardware or CPU event occurs
2. CPU consults the IDT
3. low-level assembly stub runs
4. stub saves registers and segment state
5. stub builds an `interrupt_frame`
6. stub calls `isr_common_handler`
7. C code routes to PIT, keyboard, or panic
8. PIC gets an EOI for hardware IRQs
9. assembly restores state and executes `iretd`

### Files involved

- `kernel/arch/x86/isr.asm`
- `kernel/arch/x86/irq.asm`
- `kernel/arch/x86/idt.c`

### Why there are separate `isr.asm` and `irq.asm` files

CPU exceptions and hardware interrupts are conceptually different:

- exceptions come from the CPU itself
- IRQs come from external hardware via the PIC

The common handling path is similar, but splitting them keeps the intent clearer.

## VGA Console

The console code lives in:

- `kernel/console/vga.c`

What it does:

- writes directly to VGA memory
- keeps a software cursor
- handles newline and screen scrolling
- writes fixed-position characters for the heartbeat
- prints hexadecimal values

Why this is used instead of BIOS printing:

- once the kernel is in protected mode, BIOS services are no longer the normal printing path
- direct VGA memory writes are simple and reliable for early kernel output

## PIC And PIT

### PIC

File:

- `kernel/arch/x86/pic.c`

What it does:

- remaps hardware interrupts out of the CPU exception range
- masks and unmasks IRQ lines
- sends end-of-interrupt acknowledgements

Why remapping is necessary:

- default PIC IRQ numbers overlap CPU exception vectors
- the kernel wants exceptions and hardware interrupts in separate ranges

### PIT

File:

- `kernel/arch/x86/pit.c`

What it does:

- programs the timer hardware to tick periodically
- increments a global tick count
- updates the heartbeat spinner once per second

Why the heartbeat exists:

- it proves interrupts are still firing
- it gives immediate visual feedback in QEMU

## Keyboard Driver

File:

- `kernel/arch/x86/keyboard.c`

What it does:

- reads scancodes from port `0x60`
- tracks Shift press/release state
- translates scancodes into ASCII
- echoes characters to the screen
- treats `!` as the exception self-test trigger

Why this is still simple:

- it only supports a very small part of keyboard behavior
- there is no line editing, no key-repeat logic, and no full layout support yet

## Panic And Exception Output

File:

- `kernel/debug/panic.c`

What it does:

- prints panic messages
- prints exception details from the saved interrupt frame
- halts forever afterward

What it prints now:

- interrupt vector
- error code
- `eip`
- `cs`
- `eflags`

Why this matters:

- blank screens are much harder to debug than explicit fault dumps

## PMM: Physical Memory Manager

File:

- `kernel/mm/pmm.c`

What it does:

- tracks early physical pages with a small fixed-size bitmap-style array
- uses BIOS-reported conventional memory size
- reserves low memory, kernel memory, and part of the upper conventional-memory region
- hands out page-sized allocations

Current limitations:

- only tracks a limited number of pages
- not using a full BIOS memory map
- conservative and intentionally simple

Why it still matters:

- page tables need physical pages
- task stacks need physical pages
- later memory systems depend on this layer

## kmalloc

File:

- `kernel/mm/kmalloc.c`

What it does:

- implements a simple bump allocator
- requests fresh pages from the PMM
- hands out small aligned chunks from those pages

Current limitations:

- no freeing
- no coalescing
- no variable-size reuse
- not meant to be final

Why it exists anyway:

- early kernels need some dynamic allocation before a full heap exists

## Paging

File:

- `kernel/mm/paging.c`

What it does:

- allocates a page directory
- allocates the first page table
- identity maps the first `1 MiB`
- loads `cr3`
- sets the paging bit in `cr0`
- supports mapping new pages on demand
- exposes a smoke test for non-identity mappings

Why identity mapping comes first:

- it minimizes moving parts
- the kernel can keep running at the same addresses while paging is first enabled

What the mapping smoke test proves:

- new page tables can be created dynamically
- a higher virtual address can point to a real physical page

## Tasks And Scheduler Groundwork

Files:

- `include/kernel/task.h`
- `include/kernel/sched.h`
- `kernel/sched/task.c`
- `kernel/arch/x86/switch.asm`

What exists now:

- real task structs
- named tasks for easier scheduler introspection
- per-task stack pages
- PID assignment
- fixed-size task table
- explicit idle task fallback
- first runnable task entry via assembly
- cooperative yield-based task switching
- timer-guided yield requests from PIT ticks

What does not exist yet:

- interrupt-time task switching
- PIT-driven preemptive scheduling
- a full round-robin preemptive scheduler

What the current tests prove:

- the kernel can create task records
- the kernel can jump into the first runnable task’s saved context
- the kernel can cooperatively switch from one task to another
- the timer can now ask tasks to yield after a small quantum
- the scheduler has an idle fallback when normal work is unavailable

## Why Some Parts Look "Insane"

A lot of low-level code looks strange compared to normal C programs. That is because it is doing at least one of these:

- matching exact CPU calling/stack expectations
- talking to old PC hardware through exact port numbers
- setting or testing exact bits in control registers
- working with page-aligned addresses and masks
- transitioning between processor modes

Examples:

- `or eax, 0x1` in Stage 2
  Sets the protected-mode enable bit in `cr0`

- `or $0x80000000, %eax` in paging setup
  Sets the paging enable bit in `cr0`

- `0x08` and `0x10` segment selectors
  These are GDT selector values for code and data segments

- `lidt`, `lgdt`, `iretd`
  These are privileged x86 instructions for descriptor tables and interrupt return

The code is weird because the machine interface is weird, not because the project is messy.

## Current Risks And Simplifications

The project is still making many deliberate simplifications:

- fixed-sector kernel loading
- small GDT
- tiny IDT coverage
- no full memory map parsing
- no advanced keyboard handling
- no real scheduler yet
- no user space
- no filesystem

These are acceptable for the current stage. They keep the code understandable while the fundamentals are still being built.

## Suggested Reading Order

If reading the code from scratch, this order makes the most sense:

1. `stage1.asm`
2. `stage2.asm`
3. `kernel_entry.asm`
4. `kernel/main.c`
5. `kernel/console/vga.c`
6. `kernel/arch/x86/idt.c`
7. `kernel/arch/x86/irq.asm`
8. `kernel/arch/x86/isr.asm`
9. `kernel/arch/x86/pic.c`
10. `kernel/arch/x86/pit.c`
11. `kernel/arch/x86/keyboard.c`
12. `kernel/mm/pmm.c`
13. `kernel/mm/kmalloc.c`
14. `kernel/mm/paging.c`
15. `kernel/sched/task.c`
16. `kernel/arch/x86/switch.asm`

That reading order follows the actual system bring-up path reasonably well.

## How To Keep This Guide Updated

Whenever a new subsystem is added, update this file in three places:

1. Add the subsystem to the "What it can currently do" section
2. Add or update the relevant file explanation in the file-by-file guide
3. Update the kernel bring-up order if `kernel_main` changes

If a new acronym or magic number appears, add it here instead of leaving it unexplained in scattered comments.
