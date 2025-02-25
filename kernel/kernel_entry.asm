; Multiboot header for GRUB
[BITS 32]
section .multiboot
    MULTIBOOT_MAGIC    equ 0x1BADB002
    MULTIBOOT_FLAGS    equ (1 << 0) | (1 << 1)
    MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

; Kernel stack section
section .bss
    align 16
stack_bottom:
    resb 16384   ; 16 KB of stack space
stack_top:

; Kernel entry point
section .text
    global _start
    extern kmain

_start:
    ; Multiboot compliance check
    cmp eax, 0x2BADB002
    je multiboot_ok

    ; Error handling - display detailed error
    mov dword [0xB8000], 0x4F524F45  ; 'ERRO'
    mov dword [0xB8004], 0x4F52524F  ; 'ROR'

    ; Halt
.hang:
    cli
    hlt
    jmp .hang

multiboot_ok:
    ; Disable interrupts
    cli
    
    ; Set up stack
    mov esp, stack_top
    
    ; Push multiboot information
    push ebx  ; Multiboot info pointer
    push eax  ; Multiboot magic value
    
    ; Call kernel main
    call kmain
    
    ; Halt if kernel returns
.final_hang:
    cli
    hlt
    jmp .final_hang
