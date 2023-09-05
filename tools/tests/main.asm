org 0x7C00
bits 16

entry:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    cli
    call EnableA20
    call LoadGDT

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dword 0x08:.pmode

.pmode:
    [bits 32]
    
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

    mov esi, hello
    mov bl, byte 0x1
    mov edi, 0xB8000
    cld
.loop:
    lodsb
    or al, al
    jz .done

    mov [edi], al
    inc edi
    mov [edi], bl
    inc ebx
    inc edi
    jmp .loop
.done:
    jmp word 0x18:.pmode16

.pmode16:
    [bits 16]
    mov eax, cr0
    and al, 0
    mov cr0, eax

    jmp word 0x00:.rmode
.rmode:
    mov ax, 0
    mov ds, ax
    mov ss, ax

    sti
    mov si, hello
.rloop:
    lodsb
    or al, al
    jz .rdone
    mov ah, 0x0E
    int 0x10
    jmp .rloop
.rdone:
.hlt:
    jmp .hlt

; 0x60 kbd controller data port
; 0x64 kbd controler command port
; 0xAD disable keyboard
; 0xAE enable keyboard
; 0xD0 read from port
; 0xD1 write to port
EnableA20:
    [bits 16]
    call A20WaitInput
    mov al, 0xAD
    out 0x64, al
    call A20WaitInput
    mov al, 0xD0
    out 0x64, al
    call A20WaitOutput
    in al, 0x60
    push eax

    call A20WaitInput
    mov al, 0xD1
    out 0x64, al
    call A20WaitInput
    pop eax
    or al, 2
    out 0x64, al

    call A20WaitInput
    mov al, 0xAE
    out 0x64, al
    call A20WaitInput
    ret


A20WaitInput:
    [bits 16]
    in al, 0x64
    test al, 2
    jnz A20WaitInput
    ret

A20WaitOutput:
    [bits 16]
    in al, 0x64
    test al, 1
    jz A20WaitOutput
    ret

LoadGDT:
    [bits 16]
    lgdt [gdtDescriptor]
    ret

gdt: dq 0
     
     dw 0x0FFFF
     dw 0
     db 0
     db 10011010b
     db 11001111b
     db 0
     
     dw 0x0FFFF
     dw 0
     db 0
     db 10010010b
     db 11001111b
     db 0
     
     dw 0x0FFFF
     dw 0
     db 0
     db 10011010b
     db 00001111b
     db 0
     
     dw 0x0FFFF
     dw 0
     db 0
     db 10010010b
     db 00001111b
     db 0
gdtDescriptor: dw gdtDescriptor - gdt - 1
               dd gdt

hello: db "Hello World!", 0

times 510-($-$$) db 0
dw 0xAA55 
