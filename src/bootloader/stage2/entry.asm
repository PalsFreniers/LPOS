bits 16

section .entry

extern __bss_start
extern __end
extern _init
extern start
global entry

entry:
    cli
    mov [BootDrive], dl
    mov [boot_part_off], si
    mov [boot_part_seg], di
    mov ax, ds
    mov ss, ax
    mov sp, 0xFFF0
    mov bp, sp
    
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

    mov edi, __bss_start
    mov ecx, __end
    sub ecx, edi
    mov al, 0
    cld
    rep stosb

    call _init
    
    xor edx, edx
    mov dx, [boot_part_seg]
    shl edx, 16
    mov dx, [boot_part_off]
    push edx
    
    xor edx, edx
    mov dl, [BootDrive]
    push edx
    call start

    cli
    hlt

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

BootDrive: db 0
boot_part_off: dw 0
boot_part_seg: dw 0
