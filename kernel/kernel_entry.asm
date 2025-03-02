; =============================================================================
; Enhanced Kernel Entry Point
; Date: 2025-03-01 19:09:13 UTC
; Author: opuadm
; Purpose: Advanced multiboot compliant kernel entry with hardware checks
; =============================================================================

[BITS 32]

; Multiboot constants
MULTIBOOT_MAGIC        equ 0x1BADB002
MULTIBOOT_ALIGN        equ 1<<0
MULTIBOOT_MEMINFO      equ 1<<1
MULTIBOOT_VIDMODE      equ 1<<2
MULTIBOOT_FLAGS        equ MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO | MULTIBOOT_VIDMODE
MULTIBOOT_CHECKSUM     equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; System constants
STACK_SIZE             equ 32768  ; 32KB stack
MIN_MEMORY             equ 1048576 ; Minimum 1MB memory
KERNEL_VIRTUAL_BASE    equ 0xC0000000

; Error codes
ERR_NO_MULTIBOOT      equ 0x01
ERR_NO_MEMORY         equ 0x02
ERR_NO_CPU_FEATURE    equ 0x03
ERR_INVALID_VIDEO     equ 0x04

section .multiboot
align 4
    ; Multiboot header
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
    
    ; Video mode
    dd 0                ; Header addr
    dd 0                ; Load addr
    dd 0                ; Load end addr
    dd 0                ; BSS end addr
    dd 0                ; Entry addr
    dd 0                ; Mode type
    dd 1024             ; Width
    dd 768              ; Height
    dd 32               ; Depth

section .bss
align 16
global stack_bottom
stack_bottom:
    resb STACK_SIZE     ; Reserve stack space
global stack_top
stack_top:

section .data
align 4
cpu_features:           dd 0
memory_size:           dd 0
video_mode:            dd 0
error_codes:           dd 0

; Error messages
error_msgs:
    db 'ERR: No multiboot', 0
    db 'ERR: Insufficient memory', 0
    db 'ERR: Required CPU feature missing', 0
    db 'ERR: Invalid video mode', 0

section .text
align 4
global _start
extern kmain

; CPU feature detection
check_cpu_features:
    pushad
    mov eax, 1
    cpuid
    mov [cpu_features], edx
    popad
    ret

; Memory validation
check_memory:
    pushad
    mov eax, [ebx + 8]     ; Get memory size from multiboot
    cmp eax, MIN_MEMORY
    jl .memory_error
    mov [memory_size], eax
    popad
    ret
.memory_error:
    mov dword [error_codes], ERR_NO_MEMORY
    jmp error_handler

; Video mode validation
check_video_mode:
    pushad
    mov eax, [ebx + 72]    ; Get video mode from multiboot
    test eax, eax
    jz .video_error
    mov [video_mode], eax
    popad
    ret
.video_error:
    mov dword [error_codes], ERR_INVALID_VIDEO
    jmp error_handler

; Error display routine
error_handler:
    mov edi, 0xB8000      ; Video memory
    xor ecx, ecx
    mov ah, 0x4F          ; Red on black
.display_loop:
    mov al, [error_msgs + ecx]
    test al, al
    jz .end
    mov [edi], ax
    add edi, 2
    inc ecx
    jmp .display_loop
.end:
    cli
    hlt
    jmp $

_start:
    cli                    ; Disable interrupts

    ; Validate multiboot
    cmp eax, 0x2BADB002
    jne .no_multiboot

    ; Save multiboot info
    push ebx

    ; Initialize stack
    mov esp, stack_top
    and esp, -16           ; Align stack

    ; Clear direction flag
    cld

    ; Initialize registers
    xor eax, eax
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    xor ebp, ebp

    ; Check CPU features
    call check_cpu_features

    ; Check memory
    call check_memory

    ; Check video mode
    call check_video_mode

    ; All checks passed, restore multiboot info
    pop ebx

    ; Save registers for kernel
    push ebx              ; Multiboot info
    push eax              ; Magic number

    ; Call kernel
    call kmain

    ; If kernel returns, halt
    cli
.halt:
    hlt
    jmp .halt

.no_multiboot:
    mov dword [error_codes], ERR_NO_MULTIBOOT
    jmp error_handler

; Debug support
global debug_break
debug_break:
    xchg bx, bx          ; Bochs debug breakpoint
    ret

; CPU feature bits for reference
CPU_FEATURE_FPU    equ 1 << 0
CPU_FEATURE_VME    equ 1 << 1
CPU_FEATURE_DE     equ 1 << 2
CPU_FEATURE_PSE    equ 1 << 3
CPU_FEATURE_TSC    equ 1 << 4
CPU_FEATURE_MSR    equ 1 << 5
CPU_FEATURE_PAE    equ 1 << 6
CPU_FEATURE_MCE    equ 1 << 7
CPU_FEATURE_CX8    equ 1 << 8
CPU_FEATURE_APIC   equ 1 << 9

; System state preservation
global save_system_state
save_system_state:
    pushad
    mov [system_state.eax], eax
    mov [system_state.ebx], ebx
    mov [system_state.ecx], ecx
    mov [system_state.edx], edx
    mov [system_state.esi], esi
    mov [system_state.edi], edi
    mov [system_state.ebp], ebp
    mov [system_state.esp], esp
    popad
    ret

section .data
align 4
system_state:
    .eax dd 0
    .ebx dd 0
    .ecx dd 0
    .edx dd 0
    .esi dd 0
    .edi dd 0
    .ebp dd 0
    .esp dd 0
