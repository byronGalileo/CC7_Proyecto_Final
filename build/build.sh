#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e
cd "$(dirname "$0")"

# Remove previous compiled objects and binaries
echo "Cleaning up previous build files..."
rm -f ../*/*.o ../os/*.elf ../bin/main.bin

echo "Assembling startup.s..."
arm-none-eabi-as --warn --fatal-warnings ../os/root.s -o ../os/root.o

echo "Compiling string.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../lib/string.c -o ../lib/string.o

echo "Compiling main.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../os/main.c -o ../os/main.o

echo "Compiling stdio.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../lib/stdio.c -o ../lib/stdio.o

echo "Compiling os.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../os/os.c -o ../os/os.o

echo "Linking object files..."
arm-none-eabi-gcc -T ../os/memmap.ld ../os/root.o ../os/os.o ../lib/stdio.o ../os/main.o -o ../os/main.elf -lgcc -lm -nostartfiles -mfpu=neon -mfloat-abi=hard

echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary ../os/main.elf ../bin/main.bin

echo "Disassemble the ELF file to verify addresses ..."
arm-none-eabi-objdump -D ../os/main.elf > ../os/main.list