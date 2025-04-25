CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy
CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding
LDFLAGS=-T memmap.ld

OBJS=startup.o lib.o main.o

default: process1.bin

process1.bin: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o process1.elf
	$(OBJCOPY) -O binary process1.elf process1.bin

clean:
	rm -f *.o *.elf *.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

startup.o: startup.S
	$(CC) $(CFLAGS) -c startup.S -o startup.o