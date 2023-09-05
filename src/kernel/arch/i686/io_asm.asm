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

global x86_cli
x86_cli:
    cli
    ret

global x86_sti
x86_sti:
    sti
    ret

global x86_kernel_panic
x86_kernel_panic:
    cli
    hlt
