CC = gcc
AS = nasm
LD = ld
CFLAGS = -fno-builtin -fno-exceptions -fno-stack-protector -fno-rtti -nostdlib -nodefaultlibs -Os -Wall -Wextra -m32
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -melf_i386 -Os

OBJDIR = obj
OBJS = $(OBJDIR)/boot.o $(OBJDIR)/kernel.o $(OBJDIR)/string.o $(OBJDIR)/terminal.o $(OBJDIR)/keyboard.o $(OBJDIR)/stdio.o $(OBJDIR)/gdt.o $(OBJDIR)/commands.o $(OBJDIR)/idt.o $(OBJDIR)/interrupts.o $(OBJDIR)/signal_panic.o $(OBJDIR)/syscall.o
HEADER = stdio.h io.h keyboard.h string.h terminal.h vga.h gdt.h signal_panic.h idt.h syscall.h
all: kernel.iso

$(OBJDIR)/%.o: %.c $(HEADER)
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJDIR)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

kernel.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS) 

kernel.iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue \
		--locales="" \
		--fonts="" \
		-o kernel.iso isodir

clean:
	rm -f kernel.bin
	rm -rf isodir $(OBJDIR)

run: mykernel.bin
	qemu-system-i386 -kernel mykernel.bin 

run-iso: kernel.iso
	qemu-system-i386 -cdrom kernel.iso
