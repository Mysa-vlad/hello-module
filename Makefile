# Kbuild-частина: обробляється системою збирання ядра
ifneq ($(KERNELRELEASE),)
obj-m := hello.o

# Звичайний виклик make: передаємо керування Makefile ядра
else
KDIR ?= /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

endif
