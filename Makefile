ASM=nasm
SRC_BIRS=src
BUILD_DIR=build
TOOLS_DIR=tools

.PHONY: all floppy_image kernel bootloader clean always

all: floppy_image tools_fat

floppy_image: $(BUILD_DIR)/floppy.img

$(BUILD_DIR)/floppy.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)/floppy.img bs=512 count=2880
	mkfs.fat -F 12 -n "LPOS" $(BUILD_DIR)/floppy.img
	dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/floppy.img conv=notrunc
	mcopy -i $(BUILD_DIR)/floppy.img $(BUILD_DIR)/stage2.bin "::stage2.bin"
	mcopy -i $(BUILD_DIR)/floppy.img $(BUILD_DIR)/kernel.bin "::kernel.bin"

bootloader: stage1 stage2

stage1: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: always
	$(MAKE) -C $(SRC_BIRS)/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: always
	$(MAKE) -C $(SRC_BIRS)/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(MAKE) -C $(SRC_BIRS)/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

tools_fat: $(BUILD_DIR)/tools/fat
$(BUILD_DIR)/tools/fat: always $(TOOLS_DIR)/fat/fat.c
	mkdir -p $(BUILD_DIR)/tools
	$(CC) -g -o $(BUILD_DIR)/tools/fat $(TOOLS_DIR)/fat/fat.c

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*