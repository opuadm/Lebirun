[BITS 16]
[ORG 0x7E00]

stage2:
    ; Switch to protected mode
    cli
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp CODE_SEG:init_pm

[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up stack
    mov esp, 0x90000
    
    ; Load kernel
    mov eax, 0x100000    ; Kernel destination
    mov ebx, 2           ; Starting sector
    mov ecx, 50          ; Number of sectors
    call load_kernel
    
    jmp 0x100000        ; Jump to kernel

load_kernel:
    pusha
    mov esi, eax        ; Destination address
    mov edi, ebx        ; Starting sector
    mov ecx, ecx        ; Number of sectors
    
.loop:
    pushad              ; Save all registers
    
    ; Convert LBA to CHS
    mov eax, edi
    xor edx, edx
    mov ebx, 18
    div ebx
    inc edx
    mov cl, dl          ; Sector
    
    mov eax, eax
    xor edx, edx
    mov ebx, 2
    div ebx
    mov dh, dl          ; Head
    mov ch, al          ; Cylinder
    
    ; Read sector
    mov ah, 0x02
    mov al, 1           ; One sector at a time
    mov dl, 0x80        ; Drive number
    mov bx, 0x8000      ; Temporary buffer
    int 0x13
    jc .error
    
    ; Copy to destination
    mov esi, 0x8000
    mov edi, [esp+28]   ; Get destination from saved registers
    mov ecx, 512/4      ; Copy as dwords
    rep movsd
    
    popad               ; Restore all registers
    add edi, 1          ; Next sector
    add dword [esp+28], 512  ; Next destination
    loop .loop
    
    popa
    ret

.error:
    mov si, disk_error
    call print
    jmp $

disk_error db 'Disk error!', 0

; GDT
gdt_start:
    dq 0                ; Null descriptor

gdt_code:
    dw 0xFFFF          ; Limit (bits 0-15)
    dw 0               ; Base (bits 0-15)
    db 0               ; Base (bits 16-23)
    db 10011010b       ; Access byte
    db 11001111b       ; Flags + Limit (bits 16-19)
    db 0               ; Base (bits 24-31)

gdt_data:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 512-($-$$) db 0   ; Pad to 512 bytes
