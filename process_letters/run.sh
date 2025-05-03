#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "🧹 Cleaning up previous build files..."
rm -f startup.o lib.o main.o process1.elf process1.bin

echo "🔧 Assembling startup.S..."
arm-none-eabi-as -o startup.o startup.S

echo "🧾 Compiling lib.c..."
arm-none-eabi-gcc -c -o lib.o lib.c

echo "🧾 Compiling main.c..."
arm-none-eabi-gcc -c -o main.o main.c

echo "🔗 Linking object files with memmap.ld..."
arm-none-eabi-gcc -T memmap.ld -o process1.elf startup.o lib.o main.o -lgcc -lm -nostartfiles

echo "📦 Converting ELF to raw binary..."
arm-none-eabi-objcopy -O binary process1.elf process1.bin

echo "🚀 Ejecutando process1 en QEMU (versatilepb)..."
qemu-system-arm -M versatilepb -nographic -echr 0x14 -kernel process1.elf
