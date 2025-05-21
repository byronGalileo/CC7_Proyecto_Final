#!/bin/bash

set -e

BUILD_DIR=build/os
OUTPUT=main

mkdir -p $BUILD_DIR

echo "Cleaning previous build files..."
rm -f $BUILD_DIR/*.o $BUILD_DIR/*.elf $BUILD_DIR/*.bin $BUILD_DIR/*.list

# Assemble root.s
echo "Assembling root.s..."
arm-none-eabi-as --warn --fatal-warnings os/root.s -o $BUILD_DIR/root.o

# Compile C sources
compile() {
  echo "Compiling $1..."
  arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard \
    -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding \
    $1 -o $BUILD_DIR/$(basename "$1" .c).o
}

compile os/main.c
compile os/core/os.c
compile os/core/tasks.c
compile os/core/sched.c
compile os/core/timer.c
compile lib/stdio.c
compile lib/string.c
compile lib/console.c
compile drivers/uart.c
compile drivers/io.c

# Link all object files
echo "Linking object files..."
arm-none-eabi-gcc -T os/memmap.ld \
  $BUILD_DIR/*.o \
  -o $BUILD_DIR/$OUTPUT.elf \
  -lgcc -lm -nostartfiles -mfpu=neon -mfloat-abi=hard

# Convert to binary
echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary $BUILD_DIR/$OUTPUT.elf $BUILD_DIR/$OUTPUT.bin

# Disassemble ELF
echo "Disassemble the ELF file to verify addresses..."
arm-none-eabi-objdump -D $BUILD_DIR/$OUTPUT.elf > $BUILD_DIR/$OUTPUT.list

echo "Build complete. Output in $BUILD_DIR/"
