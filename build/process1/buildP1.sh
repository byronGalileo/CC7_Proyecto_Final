#!/bin/bash

set -e

BUILD_DIR=build/process1
BIN_DIR=build/bin
OUTPUT=process1

mkdir -p $BUILD_DIR
mkdir -p $BIN_DIR

echo "Cleaning previous build files..."
rm -f $BUILD_DIR/*.o $BUILD_DIR/*.elf $BUILD_DIR/*.bin $BUILD_DIR/*.list

# Assemble startup
arm-none-eabi-as --warn --fatal-warnings process_letters/root.s -o $BUILD_DIR/root.o

# Compile sources
compile() {
  echo "Compiling $1..."
  arm-none-eabi-gcc -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard \
    -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding \
    $1 -o $BUILD_DIR/$(basename "$1" .c).o
}

compile lib/stdio.c
compile lib/string.c
compile lib/console.c
compile core/os.c
compile process_letters/process1.c
compile drivers/uart.c
compile drivers/io.c

# Link
arm-none-eabi-gcc -T process_letters/memmap.ld \
  -Wl,-e,_start \
  $BUILD_DIR/*.o \
  -o $BUILD_DIR/$OUTPUT.elf \
  -lgcc -lm -nostartfiles -mfpu=neon -mfloat-abi=hard

# Convert and dump
arm-none-eabi-objcopy -O binary $BUILD_DIR/$OUTPUT.elf $BIN_DIR/$OUTPUT.bin
arm-none-eabi-objdump -D $BUILD_DIR/$OUTPUT.elf > $BUILD_DIR/$OUTPUT.list

echo "Build complete: $BIN_DIR/$OUTPUT.bin"