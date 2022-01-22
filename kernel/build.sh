#!/bin/sh

rm -rf $(find . -name "*.o") *.elf

for f in $(find . -name "*.c"); do
	gcc -Wall -Wextra -Werror -g -O2 -pipe -Ilibk -ansi \
	    -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 \
     	-mno-red-zone -mcmodel=kernel -ffreestanding \
     	-fno-stack-protector -fno-omit-frame-pointer -fno-pic \
     	-fno-pie -Ilibk -c "$f" -o "${f%.*}.o"
done

find . -name "*.blob" -exec nasm -fbin -g -O2 {} -o {}.bin \;
find . -name "*.asm" -exec nasm -felf64 -g -O2 {} \;

ld -O2 -nostdlib -no-pie -z max-page-size=0x1000 -Tlinker.ld $(find . -name "*.o") -o oxyb.elf
