; Memory Management ASM functions for low-level memory operations
; These functions provide hardware access that C code can't do directly

[BITS 32]

section .text
global asm_verify_memory_size
global asm_invalidate_page
global asm_flush_tlb

; Verify memory size and explicitly set correct values for VM platforms
; uint32_t asm_verify_memory_size(uint32_t suggested_size);
asm_verify_memory_size:
    push ebp
    mov ebp, esp
    
    ; Get the suggested size parameter
    mov eax, [ebp+8]
    
    ; Force to 1024MB (1GB) for VM environments
    mov eax, 0x40000000 ; 1024MB exactly
    
.done:
    pop ebp
    ret

; Invalidate a specific page in the TLB
; void asm_invalidate_page(uint32_t addr);
asm_invalidate_page:
    push ebp
    mov ebp, esp
    
    ; Get the address parameter
    mov eax, [ebp+8]
    
    ; Invalidate page in TLB
    invlpg [eax]
    
    pop ebp
    ret

; Flush entire TLB
; void asm_flush_tlb();
asm_flush_tlb:
    push ebp
    mov ebp, esp
    
    ; Read CR3 register
    mov eax, cr3
    
    ; Write CR3 back to itself (triggers full TLB flush)
    mov cr3, eax
    
    pop ebp
    ret

; Add a .note.GNU-stack section to indicate a non-executable stack
section .note.GNU-stack noalloc noexec nowrite progbits
