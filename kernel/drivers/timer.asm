; Timer-related assembly functions for accurate timing
[BITS 32]

extern timer_ticks

section .text
global timer_hw_init
global timer_wait_next_tick

; Simple hardware init - just basic setup
timer_hw_init:
    push ebp
    mov ebp, esp
    
    ; Basic PIT setup only - no complex checks
    mov al, 0x34        ; Channel 0, rate generator 
    out 0x43, al
    mov al, 0xFF        ; Maximum count value
    out 0x40, al
    out 0x40, al
    
    mov esp, ebp
    pop ebp
    ret

; Simple wait - avoid using HLT
timer_wait_next_tick:
    push ebp
    mov ebp, esp
    mov ecx, 100        ; Small delay loop
.delay:
    in al, 0x80         ; Safe port read for delay
    loop .delay
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
