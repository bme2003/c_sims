# BIOS Bootloader Starter

This directory contains a minimal from-scratch boot path:

- `stage1.asm`: 512-byte boot sector loaded by the BIOS at `0x7C00`
- `stage2.asm`: second-stage loader loaded by Stage 1 at `0x8000`
- `kernel_entry.asm`: 32-bit kernel entry stub
- `kernel.c`: tiny freestanding kernel that writes to VGA text memory
- `linker.ld`: linker script that places the kernel at physical address `0x10000`

## Boot flow

1. BIOS loads `stage1.bin` to `0x7C00` and jumps to it.
2. Stage 1 prints a short message and loads 4 sectors of Stage 2 from disk.
3. Stage 2 prints a short message and loads the kernel from fixed sectors.
4. Stage 2 enables A20, loads a tiny GDT, and enters 32-bit protected mode.
5. Stage 2 jumps to the kernel entry point at `0x10000`.
6. The kernel receives basic boot information and writes status text to VGA text memory.

## Required tools

- `nasm`
- `qemu-system-i386`
- an ELF cross-compiler toolchain
  - either `i686-elf-gcc`, `i686-elf-ld`, `i686-elf-objcopy`
  - or `x86_64-elf-gcc`, `x86_64-elf-ld`, `x86_64-elf-objcopy`

On macOS, the easiest current Homebrew path is usually:

```sh
brew install nasm qemu x86_64-elf-gcc
```

Homebrew’s `x86_64-elf-gcc` formula was formerly named `i386-elf-gcc`, and it depends on `x86_64-elf-binutils`, so the linker tools normally arrive with it. The Makefile will automatically use `i686-elf-*` if present, otherwise it will fall back to `x86_64-elf-*`. See Homebrew’s formula pages for [`nasm`](https://formulae.brew.sh/formula/nasm), [`qemu`](https://formulae.brew.sh/formula/qemu), and [`x86_64-elf-gcc`](https://formulae.brew.sh/formula/x86_64-elf-gcc). OSDev also recommends using a true ELF cross-compiler for OS work: [GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross_Compiler).

## Build and run

From the repo root:

```sh
make -C bootloader
make -C bootloader run
```

From inside the `bootloader/` directory:

```sh
make
make run
```

## Notes

- This project uses fixed disk sectors for simplicity.
- Stage 2 is expected to fit in 4 sectors.
- The kernel is currently loaded from BIOS sector 6 and is expected to fit within the configured kernel sector count in the loader and Makefile.
- This is a learning scaffold, not yet a flexible bootloader.

Good next steps are:

1. Add retries and reset logic around BIOS disk reads.
2. Clear the screen and add cleaner text output helpers.
3. Pass boot information from Stage 2 to the kernel.
4. Replace fixed-sector kernel loading with ELF parsing.
5. Add an IDT and interrupts after the kernel boots reliably.
