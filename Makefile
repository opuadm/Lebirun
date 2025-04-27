# Compiler and linker settings
ASM=nasm
CC=gcc
LD=ld

# Compiler flags
CFLAGS=-m32 \
	-fno-pie \
	-fno-stack-protector \
	-nostdlib \
	-fno-builtin \
	-fno-exceptions \
	-Wall \
	-Wextra \
	-I./kernel

# Linker flags
LDFLAGS=-m elf_i386 \
	-nostdlib \
	-Tlinker.ld

# Object files - added new assembly files for memory and timer
KERNEL_OBJS=build/kernel_entry.o \
	build/kernel.o \
	build/drivers/keyboard.o \
	build/drivers/screen.o \
	build/shell/shell.o \
	build/drivers/power.o \
	build/drivers/mm.o \
	build/drivers/mm_asm.o \
	build/interrupts/idt.o \
	build/interrupts/isr.o \
	build/interrupts/interrupt.o \
	build/interrupts/exceptions.o \
	build/interrupts/isr_asm.o \
	build/interrupts/idt_checker.o \
	build/drivers/timer.o \
	build/drivers/timer_asm.o

# Store the build start time (in milliseconds) - using a more robust approach
START_TIME_SEC := $(shell date +%s)
START_TIME_NSEC := $(shell date +%N | sed 's/^0*//')
START_TIME_MS := $(shell expr $(START_TIME_SEC) \* 1000 + $(START_TIME_NSEC) / 1000000)

# Default target
all: build/lebirun.iso
	$(eval END_TIME_SEC := $(shell date +%s))
	$(eval END_TIME_NSEC := $(shell date +%N | sed 's/^0*//'))
	$(eval END_TIME_MS := $(shell expr $(END_TIME_SEC) \* 1000 + $(END_TIME_NSEC) / 1000000))
	$(eval DIFF_MS := $(shell expr $(END_TIME_MS) - $(START_TIME_MS)))
	$(eval DIFF_SEC := $(shell expr $(DIFF_MS) / 1000))
	$(eval DIFF_REMAINDER_MS := $(shell expr $(DIFF_MS) % 1000))
	@if [ $(DIFF_MS) -ge 1000 ]; then \
		echo "Build completed in $(DIFF_SEC).$(shell printf "%03d" $(DIFF_REMAINDER_MS)) seconds"; \
	else \
		echo "Build completed in $(DIFF_MS) milliseconds"; \
	fi

# Compile assembly files
build/kernel_entry.o: kernel/kernel_entry.asm
	mkdir -p build
	$(ASM) -f elf32 -o $@ $<

# Compile ISR assembly file
build/interrupts/isr_asm.o: kernel/interrupts/isr.asm
	mkdir -p build/interrupts
	$(ASM) -f elf32 -o $@ $<

# Compile mm.asm
build/drivers/mm_asm.o: kernel/drivers/mm.asm
	mkdir -p build
	$(ASM) -f elf32 -o $@ $<

# Compile timer.asm
build/drivers/timer_asm.o: kernel/drivers/timer.asm
	mkdir -p build/drivers
	$(ASM) -f elf32 -o $@ $<

# Compile C files
build/%.o: kernel/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile C files in subdirectories
build/%.o: kernel/**/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# Link kernel
build/kernel.bin: $(KERNEL_OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

# Create bootable ISO
build/lebirun.iso: build/kernel.bin
	mkdir -p build/iso/boot/grub
	cp build/kernel.bin build/iso/boot/
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/iso

# Clean build artifacts
clean:
	rm -rf build/*

# Phony targets
.PHONY: all clean