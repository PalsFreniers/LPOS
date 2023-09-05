#!/usr/bin/bash

set -e
if [ $# -le 1 ]; then
    echo "Usage: generate_isr.sh <isr.c> <isr.inc>"
    exit 1
fi

ISRS_GEN_C=$1
ISRS_GEN_ASM=$2

ISRS_WITH_ERR="8 10 11 12 13 14 17 21 29 30"

echo "// !!! This is autogen !!!" > $ISRS_GEN_C
echo "#include \"idt.h\"" >> $ISRS_GEN_C
echo "#include \"gdt.h\"" >> $ISRS_GEN_C
echo "" >> $ISRS_GEN_C
for i in $(seq 0 255); do
    echo "void __attribute((cdecl)) i686_ISR${i}();" >> $ISRS_GEN_C
done
echo "" >> $ISRS_GEN_C
echo "void i686_ISR_InitGate() {" >> $ISRS_GEN_C
for i in $(seq 0 255); do
    echo "    i686_IDT_SetGate(${i}, i686_ISR${i}, I686_GDT_CODE_SEGMENT, IDT_FLAG_RING0 | IDT_FLAG_GATE_32B_INT);" >> $ISRS_GEN_C
done
echo "}" >> $ISRS_GEN_C

echo "; !!! This is autogen !!!" > $ISRS_GEN_ASM
for i in $(seq 0 255); do
    if echo "$ISRS_WITH_ERR" | grep -q "\b${i}\b"; then
        echo "int_error ${i}" >> $ISRS_GEN_ASM
    else
        echo "int_noerror ${i}" >> $ISRS_GEN_ASM
    fi
done
