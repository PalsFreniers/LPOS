%macro x86_enterRM 0
    [bits 32]
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
%endmacro

%macro x86_enterPM 0
    [bits 16]
    cli
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp dword 0x08:.pmode32
.pmode32:
    [bits 32]
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
%endmacro

%macro x86_lineartosegoffaddr 4
    mov %3, %1
    shr %3, 4
    mov %2, %4
    mov %3, %1
    and %3, 0xF
%endmacro

global x86_outb
x86_outb:
    [bits 32]
    mov dx, [esp+4]
    mov al, [esp+8]
    out dx, al
    ret

global x86_inb
x86_inb:
    [bits 32]
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret


global x86_Get_Drive_Param
x86_Get_Drive_Param:
    [bits 32]
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame
    
    x86_enterRM
    ; save regs
    push es
    push bx
    push esi
    push di
    

    ; call int13h
    mov dl, [bp + 8]    ; dl - disk drive
    mov ah, 0x08
    mov di, 0           ; es:di - 0000:0000
    mov es, di
    stc
    int 0x13

    ; return
    mov eax, 1
    sbb eax, 0

    ; out params
    x86_lineartosegoffaddr [bp + 12], es, esi, si
    mov es:[si], bl

    mov bl, ch          ; cylinders - lower bits in ch
    mov bh, cl          ; cylinders - upper bits in cl (6-7)
    shr bh, 6
    inc bx
    x86_lineartosegoffaddr [bp + 16], es, esi, si
    mov es:[si], bx

    xor ch, ch          ; sectors - lower 5 bits in cl
    and cl, 3Fh
    x86_lineartosegoffaddr [bp + 20], es, esi, si
    mov es:[si], cx

    mov cl, dh         ; heads - dh
    inc cx
    x86_lineartosegoffaddr [bp + 24], es, esi, si
    mov es:[si], cx


    ; restore regs
    pop di
    pop esi
    pop bx
    pop es
    
    push eax
    x86_enterPM
    ; restore old call frame
    pop eax
    mov esp, ebp
    pop ebp
    ret

global x86_Disk_Reset
x86_Disk_Reset:
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame
    
    x86_enterRM

    mov ah, 0
    mov dl, [bp + 8]    ; dl - drive
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   
    push eax
    x86_enterPM
    pop eax
    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

global x86_Disk_Read
x86_Disk_Read:
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    x86_enterRM
    ; save modified regs
    push ebx
    push es

    ; setup args
    mov dl, [bp + 8]    ; dl - drive

    mov ch, [bp + 12]    ; ch - cylinder (lower 8 bits)
    mov cl, [bp + 13]    ; cl - cylinder to bits 6-7
    shl cl, 6
    
    mov al, [bp + 16]    ; cl - sector to bits 0-5
    and al, 0x3F
    or cl, al

    mov dh, [bp + 20]   ; dh - head

    mov al, [bp + 24]   ; al - count

    x86_lineartosegoffaddr [bp + 28], es, ebx, bx
    

    ; call int13h
    mov ah, 0x02
    stc
    int 0x13

    ; set return value
    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   
    ; restore regs
    pop es
    pop ebx
    push eax
    x86_enterPM
    pop eax
    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

global x86_Video_Get_VBE_Info
x86_Video_Get_VBE_Info:
    push ebp
    mov ebp, esp
    x86_enterRM

    push ebp
    push edi
    push es

    x86_lineartosegoffaddr [bp + 8], es, edi, di
    mov eax, 0x4F00
    int 0x10

    pop es
    pop edi
    pop ebp

    push eax
    x86_enterPM
    pop eax
    mov esp, ebp
    pop ebp
    ret

global x86_Video_Get_VBE_Mode_Info
x86_Video_Get_VBE_Mode_Info:
    push ebp
    mov ebp, esp
    x86_enterRM

    push ebp
    push edi
    push ecx
    push es

    x86_lineartosegoffaddr [bp + 8], es, edi, di
    mov cx, [bp + 12]
    mov eax, 0x4F01
    int 0x10

    pop es
    pop ecx
    pop edi
    pop ebp
    
    push eax
    x86_enterPM
    pop eax
    mov esp, ebp
    pop ebp
    ret

global x86_Video_Set_VBE_Mode
x86_Video_Set_VBE_Mode:
    push ebp
    mov ebp, esp
    x86_enterRM

    push ebp
    push edi
    push ebx
    push es

    mov ax, 0
    mov es, ax
    mov edi, 0
    mov bx, [bp + 8]
    mov eax, 0x4F02
    int 0x10

    pop es
    pop ebx
    pop edi
    pop ebp

    push eax
    x86_enterPM
    pop eax
    mov esp, ebp
    pop ebp
    ret
