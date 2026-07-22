CC = i686-elf-gcc
AS = i686-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc

OBJDIR = obj
OBJS = $(OBJDIR)/boot.o $(OBJDIR)/kernel.o $(OBJDIR)/string.o $(OBJDIR)/terminal.o $(OBJDIR)/keyboard.o $(OBJDIR)/stdio.o

all: kernel.iso

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $< -o $@

kernel.bin: $(OBJS) linker.ld
	$(CC) -T linker.ld -o $@ $(OBJS) $(LDFLAGS)

kernel.iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o kernel.iso isodir

clean:
	rm -f kernel.bin kernel.iso
	rm -rf isodir $(OBJDIR)
