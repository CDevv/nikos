GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
	obj/common/screen.o \
	obj/gdt.o \
	obj/main.o

all: nikos.iso

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<
obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

nikos.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

nikos.iso: nikos.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp nikos.bin iso/boot/nikos.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "nikOS" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/nikos.bin'    >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=nikos.iso iso

clean:
	rm -r iso
	rm -r obj
	rm nikos.bin
	rm nikos.iso