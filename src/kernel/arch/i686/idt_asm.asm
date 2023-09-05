[bits 32]

global i686_load_idt
i686_load_idt:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret
