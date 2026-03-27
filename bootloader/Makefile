ASM ?= nasm
QEMU ?= qemu-system-i386

ifneq ($(origin CROSS_PREFIX), undefined)
CC := $(CROSS_PREFIX)-gcc
LD := $(CROSS_PREFIX)-ld
OBJCOPY := $(CROSS_PREFIX)-objcopy
else ifeq ($(shell command -v i686-elf-gcc >/dev/null 2>&1; echo $$?),0)
CC := i686-elf-gcc
LD := i686-elf-ld
OBJCOPY := i686-elf-objcopy
else ifeq ($(shell command -v x86_64-elf-gcc >/dev/null 2>&1; echo $$?),0)
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
OBJCOPY := x86_64-elf-objcopy
else
CC := missing-cross-gcc
LD := missing-cross-ld
OBJCOPY := missing-cross-objcopy
endif

CFLAGS = -m32 -ffreestanding -fno-pic -fno-stack-protector -nostdlib -nostartfiles -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

BUILD_DIR = build
STAGE1_BIN = $(BUILD_DIR)/stage1.bin
STAGE2_BIN = $(BUILD_DIR)/stage2.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
IMAGE = $(BUILD_DIR)/os-image.bin

STAGE2_SECTORS = 4
KERNEL_SECTORS = 8

.PHONY: all run clean check-tools

all: check-tools $(IMAGE)

check-tools:
	@command -v $(ASM) >/dev/null 2>&1 || { echo "Missing assembler: $(ASM)"; echo "Install NASM, for example: brew install nasm"; exit 1; }
	@command -v $(CC) >/dev/null 2>&1 || { echo "Missing cross-compiler: $(CC)"; echo "On macOS/Homebrew, install: brew install x86_64-elf-gcc"; echo "If you want to force a toolchain, run: make CROSS_PREFIX=x86_64-elf"; exit 1; }
	@command -v $(LD) >/dev/null 2>&1 || { echo "Missing linker: $(LD)"; echo "This normally comes with the cross-compiler package."; exit 1; }
	@command -v $(OBJCOPY) >/dev/null 2>&1 || { echo "Missing objcopy: $(OBJCOPY)"; echo "This normally comes with the cross-compiler package."; exit 1; }

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(STAGE1_BIN): stage1.asm | $(BUILD_DIR)
	$(ASM) -f bin stage1.asm -o $(STAGE1_BIN)

$(STAGE2_BIN): stage2.asm | $(BUILD_DIR)
	$(ASM) -f bin stage2.asm -o $(STAGE2_BIN)

$(BUILD_DIR)/kernel_entry.o: kernel_entry.asm | $(BUILD_DIR)
	$(ASM) -f elf32 kernel_entry.asm -o $(BUILD_DIR)/kernel_entry.o

$(BUILD_DIR)/kernel.o: kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c kernel.c -o $(BUILD_DIR)/kernel.o

$(KERNEL_ELF): $(BUILD_DIR)/kernel_entry.o $(BUILD_DIR)/kernel.o linker.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $(BUILD_DIR)/kernel_entry.o $(BUILD_DIR)/kernel.o

$(KERNEL_BIN): $(KERNEL_ELF) | $(BUILD_DIR)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

$(IMAGE): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN) | $(BUILD_DIR)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=$(STAGE1_BIN) of=$(IMAGE) conv=notrunc
	dd if=$(STAGE2_BIN) of=$(IMAGE) bs=512 seek=1 conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) bs=512 seek=$$(($(STAGE2_SECTORS) + 1)) conv=notrunc

run: check-tools $(IMAGE)
	$(QEMU) -drive format=raw,file=$(IMAGE),if=floppy

clean:
	rm -rf $(BUILD_DIR)
