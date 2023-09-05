run: 32BMode.bin
	cp 32BMode.bin 32BMode.img
	truncate -s 1440k 32BMode.img
	qemu-system-i386 -fda 32BMode.img
	rm 32BMode.bin

32BMode.bin:
	nasm main.asm -f bin -o 32BMode.bin

clean:
	rm -rf 32BMode.*
