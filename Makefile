# Compiler and linker settings
ASM=nasm
CC=gcc
LD=ld

# Compiler flags
CFLAGS=-m32 \
	-fno-pie \
	-fno-stack-protector \
	-nostdlib \
	-nostdinc \
	-fno-builtin \
	-fno-exceptions \
	-Wall \
	-Wextra \
	-I./kernel

# Linker flags
LDFLAGS=-m elf_i386 \
	-nostdlib \
	-Tlinker.ld

# Object files
KERNEL_OBJS=build/kernel_entry.o \
	build/kernel.o \
	build/keyboard.o \
	build/screen.o \
	build/shell.o

# Default target
all: build/lebirun.iso

# Compile assembly files
build/kernel_entry.o: kernel/kernel_entry.asm
	$(ASM) -f elf32 -o $@ $<

# Compile C files
build/%.o: kernel/%.c
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

# Create GRUB configuration
create_grub_cfg:
	cp grub.cfg build/iso/boot/grub/grub.cfg

# Clean build artifacts
clean:
	rm -rf build/*

# Phony targets
.PHONY: all clean
