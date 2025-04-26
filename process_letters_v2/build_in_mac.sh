#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Remove previous compiled objects and binaries
echo "Cleaning up previous build files..."
rm -f *.o *.elf *.bin

echo "Assembling startup.s..."
arm-none-eabi-as --warn --fatal-warnings root.s -o root.o

echo "Compiling string.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding string.c -o string.o

echo "Compiling process1.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding process1.c -o process1.o

echo "Compiling stdio.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding stdio.c -o stdio.o

echo "Compiling os.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding os.c -o os.o

echo "Linking object files..."
arm-none-eabi-gcc -T memmap root.o os.o stdio.o process1.o -o process1.elf -lgcc -lm -nostartfiles -mfpu=neon -mfloat-abi=hard

echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary process1.elf process1.bin

echo "Disassemble the ELF file to verify addresses ..."
arm-none-eabi-objdump -D process1.elf > process1.list