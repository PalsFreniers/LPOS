#!/bin/sh
make clean
make
qemu-system-i386 -fda build/floppy.img
