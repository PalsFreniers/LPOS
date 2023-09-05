    target remote | qemu-system-i386 -S -gdb stdio -m 32 -hda /home/tidian/Documents/code/asm/LPOS/LPOS/build/i686_debug/image.img
    b *0x7c00
    layout asm
