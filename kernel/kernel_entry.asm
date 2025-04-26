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
global cpu_features
cpu_features:           dd 0
global memory_size
memory_size:           dd 0
global video_mode
video_mode:            dd 0
global error_codes
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

; Memory validation and extraction - completely rewritten
check_memory:
    pushad
    
    ; Check if memory info is available in multiboot flags
    mov eax, [ebx]
    bt eax, 0           ; Test bit 0 (memory info present)
    jc .use_mem_info    ; If set, use mem_lower/mem_upper
    
    bt eax, 6           ; Test bit 6 (mmap info present)
    jc .use_mmap        ; If set, use memory map
    
    ; If we don't have either memory info, use a conservative default
    mov dword [memory_size], 16 * 1024 * 1024  ; 16MB default
    jmp .done
    
.use_mem_info:
    ; Get memory size from mem_lower and mem_upper (in KB)
    mov eax, [ebx + 4]  ; mem_lower in KB (usually 640)
    mov edx, [ebx + 8]  ; mem_upper in KB (extends beyond 1MB)
    
    ; Convert to bytes and add together
    shl eax, 10         ; Convert to bytes (mem_lower)
    shl edx, 10         ; Convert to bytes (mem_upper)
    add eax, edx
    add eax, 1024 * 1024 ; Add 1MB (mem_upper starts counting after 1MB)
    
    ; Store memory size
    mov [memory_size], eax
    jmp .done
    
.use_mmap:
    ; Initialize memory counter
    xor edi, edi        ; Total memory size
    
    ; Get memory map address and length
    mov esi, [ebx + 44] ; mmap_addr
    mov ecx, [ebx + 48] ; mmap_length
    add ecx, esi        ; End of mmap
    
.mmap_loop:
    cmp esi, ecx        ; Check if we've reached the end
    jge .mmap_done
    
    ; Check entry type (1 = available RAM)
    mov edx, [esi + 16] ; entry type
    cmp edx, 1
    jne .next_mmap_entry
    
    ; Get base address and length of this memory region
    ; Note: We only use the lower 32 bits for simplicity
    ; This should be fine for our purposes (up to 4GB RAM)
    mov eax, [esi + 8]  ; base_addr (low)
    mov edx, [esi + 0]  ; length (low)
    
    ; Add this region's size to total
    ; But only if it starts below 4GB
    test dword [esi + 12], 0xFFFFFFFF  ; base_addr (high)
    jnz .next_mmap_entry  ; Skip if base_addr is >= 4GB
    
    ; For regions that start after the first 1MB:
    add edi, edx

.next_mmap_entry:
    ; Move to next entry (size + 4 bytes)
    mov eax, [esi]      ; size field
    add eax, 4          ; add 4 for the size field itself
    add esi, eax        ; move to next entry
    jmp .mmap_loop
    
.mmap_done:
    ; Check if we found any memory
    test edi, edi
    jz .use_default
    
    ; Cap memory at 4GB-1MB for safety
    mov eax, 0xFFF00000  ; 4GB - 1MB
    cmp edi, eax
    jbe .store_mmap_size
    mov edi, eax
    
.store_mmap_size:
    ; Store memory size, ensuring at least 16MB
    mov [memory_size], edi
    cmp edi, 16 * 1024 * 1024
    ja .done
    
.use_default:
    ; Use a safe default if memory detection failed or found too little
    mov dword [memory_size], 16 * 1024 * 1024  ; 16MB default
    
.done:
    ; Verify we have at least minimum memory
    cmp dword [memory_size], MIN_MEMORY
    ja .memory_ok
    mov dword [memory_size], MIN_MEMORY
    
.memory_ok:
    popad
    ret

; Video mode validation
check_video_mode:
    pushad
    ; Check for video info flag
    mov eax, [ebx]
    bt eax, 12
    jnc .video_default
    
    ; Get VBE info
    mov eax, [ebx + 72]
    test eax, eax
    jz .video_default
    mov [video_mode], eax
    popad
    ret
    
.video_default:
    ; Default to basic mode
    mov dword [video_mode], 1
    popad
    ret

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
    ; Disable interrupts during initialization
    cli
    
    ; Check multiboot signature
    cmp eax, 0x2BADB002
    jne .no_multiboot
    
    ; Set up stack
    mov esp, stack_top
    and esp, -16           ; Align stack to 16 bytes
    
    ; Clear direction flag
    cld
    
    ; Save multiboot info pointer
    push ebx
    
    ; Extract memory information - completely rewritten
    call check_memory
    
    ; Check video mode
    pop ebx              ; Restore multiboot info pointer
    push ebx             ; Save it again for later
    call check_video_mode
    
    ; Call kernel
    mov eax, [memory_size]
    pop ebx              ; Restore multiboot info pointer
    push ebx             ; Pass multiboot info as 2nd parameter
    push eax             ; Pass memory size as 1st parameter
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

; Add a .note.GNU-stack section to indicate a non-executable stack
section .note.GNU-stack noalloc noexec nowrite progbits