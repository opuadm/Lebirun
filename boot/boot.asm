[BITS 16]
[ORG 0x7C00]

start:
    cli
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Load stage 2
    mov ah, 0x02
    mov al, 1       ; Number of sectors
    mov ch, 0       ; Cylinder number
    mov cl, 2       ; Sector number
    mov dh, 0       ; Head number
    mov dl, 0x80    ; Drive number
    mov bx, stage2  ; Buffer
    int 0x13
    jc error

    jmp stage2

error:
    mov si, error_msg
    call print
    jmp $

print:
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    ret

error_msg db 'Boot error!', 0

times 510-($-$$) db 0
dw 0xAA55             ; Boot signature
