[bits 32]

global i686_Load_GDT
i686_Load_GDT:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lgdt [eax]
    mov eax, [ebp + 12]
    push eax
    push .reloadseg
    retf

.reloadseg:
    mov ax, [ebp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax,
    mov ss, ax

    mov esp, ebp
    pop ebp
    ret
