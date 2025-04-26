[BITS 32]

; External functions
[EXTERN isr_handler]
[EXTERN irq_handler]

; Helper macro for ISRs that don't push an error code
%macro ISR_NOERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        cli                 ; Disable interrupts
        push dword 0        ; Push dummy error code
        push dword %1       ; Push interrupt number
        jmp isr_common_stub ; Go to common handler
%endmacro

; Helper macro for ISRs that do push an error code
%macro ISR_ERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        cli                 ; Disable interrupts
        push dword %1       ; Push interrupt number
        jmp isr_common_stub ; Go to common handler
%endmacro

; Helper macro for IRQ handlers
%macro IRQ 2
    [GLOBAL irq%1]
    irq%1:
        cli                 ; Disable interrupts
        push dword 0        ; Push dummy error code
        push dword %2       ; Push interrupt number
        jmp irq_common_stub ; Go to common handler
%endmacro

; Define the CPU exception ISRs
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; Define the IRQ handlers
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Common ISR handler stub
isr_common_stub:
    ; Save registers
    pusha
    mov ax, ds
    push eax

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    push esp      ; Pass pointer to stack as argument
    call isr_handler
    add esp, 4    ; Clean up passed argument

    ; Restore registers
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa

    ; Remove error code and interrupt number
    add esp, 8
    sti           ; Re-enable interrupts
    iret          ; Return from interrupt

; Common IRQ handler stub
irq_common_stub:
    ; Save registers
    pusha
    mov ax, ds
    push eax

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    push esp      ; Pass pointer to stack as argument
    call irq_handler
    add esp, 4    ; Clean up passed argument

    ; Restore registers
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa

    ; Remove error code and interrupt number
    add esp, 8
    sti           ; Re-enable interrupts
    iret          ; Return from interrupt

; Load the IDT
[GLOBAL idt_load]
idt_load:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]  ; Get the pointer to the IDT
    lidt [eax]        ; Load the IDT
    pop ebp
    ret

; Add a .note.GNU-stack section to indicate a non-executable stack
section .note.GNU-stack noalloc noexec nowrite progbits
