#!/bin/bash

QEMU_ARGS="-S -gdb stdio -m 32"

if [ "$#" -le 1 ]; then
    echo "Usage: ./debug.sh <image_type> <image>"
    exit 1
fi

case "$1" in
    "floppy")   QEMU_ARGS="${QEMU_ARGS} -fda $PWD/$2"
    ;;
    "disk")     QEMU_ARGS="${QEMU_ARGS} -hda $PWD/$2"
    ;;
    *)          echo "Unknown image type $1."
                exit 2
esac

# b *0x7c00
# layout asm
cat > debug_script.gdb << EOF
    target remote | qemu-system-i386 $QEMU_ARGS
    b *0x7c00
    layout asm
EOF

gdb -x debug_script.gdb

