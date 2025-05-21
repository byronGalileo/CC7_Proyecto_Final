#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e
cd "$(dirname "$0")"

# Remove previous compiled objects and binaries
echo "Cleaning up previous build files..."
rm -f ../process_numbers/*.o ../process_numbers/*.elf ../bin/process2.bin

echo "Assembling startup.s..."
arm-none-eabi-as --warn --fatal-warnings ../process_numbers/root.s -o ../process_numbers/root.o

echo "Compiling string.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../lib/string.c -o ../lib/string.o

echo "Compiling process2.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../process_numbers/process2.c -o ../process_numbers/process2.o

echo "Compiling stdio.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../lib/stdio.c -o ../lib/stdio.o

echo "Compiling os.c..."
arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding ../lib/os.c -o ../lib/os.o

echo "Linking object files..."
arm-none-eabi-gcc -T ../process_numbers/memmap.ld ../process_numbers/root.o ../lib/os.o ../lib/stdio.o ../process_numbers/process2.o -o ../process_numbers/process2.elf -lgcc -lm -nostartfiles -mfpu=neon -mfloat-abi=hard

echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary ../process_numbers/process2.elf ../bin/process2.bin

echo "Disassemble the ELF file to verify addresses ..."
arm-none-eabi-objdump -D ../process_numbers/process2.elf > ../process_numbers/process2.list