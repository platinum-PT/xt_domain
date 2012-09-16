ifneq ($(KERNELRELEASE),)
obj-m := xt_domain.o

else

KERNEL_SRC ?= $(firstword $(wildcard /lib/modules/$(shell uname -r)/build /usr/src/linux))
ifeq ($(KERNEL_SRC),)
$(error You need to define KERNEL_SRC)
endif

ifneq ($wildcard $(KERNEL_SRC)/include/linux/modversions.h),)
MODVERSIONS = -DMODVERSIONS
endif

_KVER = $(strip $(shell cat $(KERNEL_SRC)/Makefile | grep -e '^VERSION' | cut -d"=" -f2))
_KPL = $(strip $(shell cat $(KERNEL_SRC)/Makefile | grep -e '^PATCHLEVEL' | cut -d"=" -f2))
_KSUB = $(strip $(shell cat $(KERNEL_SRC)/Makefile | grep -e '^SUBLEVEL' | cut -d"=" -f2))
KERNEL_SERIES=$(_KVER).$(_KPL)

ifeq ($(KERNEL_SERIES), 2.6)
	TARGET=xt_domain.ko
else
	TARGET=xt_domain.o
endif

SED = sed
IPTABLES_BIN = iptables

ifndef $(IPTABLES_SRC)
IPTVER = \
	$(shell $(IPTABLES_BIN) --version | $(SED) -e 's/^iptables v//')
IPTABLES_SRC = $(wildcard /usr/src/iptables-$(IPTVER))
endif

ifeq ($(IPTABLES_SRC),)
$(warning You need to install iptables sources and maybe set IPTABLES_SRC)
endif

IPTABLES_INCLUDE = -I$(IPTABLES_SRC)/include

ifneq ($(IPTVER),)
	IPTABLES_VERSION = $(IPTVER)
else
	IPTABLES_VERSION = $(shell cat $(IPTABLES_SRC)/Makefile | grep -e '^IPTABLES_VERSION:=' | cut -d"=" -f2)
endif

IPTABLES_OPTION = -DIPTABLES_VERSION=\"$(IPTABLES_VERSION)\"

CC = gcc
CFLAGS = -O3 -Wall



all: modules libxt_domain.so

modules: $(TARGET)

xt_domain.o: xt_domain.h xt_domain.c
	$(CC) $(CFLAGS) -I$(KERNEL_SRC)/include -c xt_domain.c -D__KERNEL__ -DMODULE $(MODVERSIONS)

xt_domain.ko: xt_domain.h xt_domain.c
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules


libxt_domain.so: libxt_domain.c xt_domain.h
	$(CC) $(CFLAGS) $(IPTABLES_OPTION) $(IPTABLES_INCLUDE) -fPIC -c libxt_domain.c
	$(CC) -shared -o libxt_domain.so libxt_domain.o

clean:
	rm -rf *.o *.so *.ko .*.cmd *.mod.c *.symvers modules.order .*.d .tmp*

install: all
	cp libxt_domain.so /lib/xtables/
	cp $(TARGET) /lib/modules/`uname -r`/kernel/net/ipv4/netfilter/
	depmod -a
endif
