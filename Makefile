
ifneq ($(KERNELRELEASE),)

coolrfreq-y = turbofreq.o pstate_user.o

obj-m  := coolrfreq.o

else

KERNELDIR := /lib/modules/`uname -r`/build 
#CC := 
#ARCH :=

all::
	$(MAKE) -C $(KERNELDIR) M=$(PWD) # CC=$(CC)  ARCH=$(ARCH) 

clean::
	rm -f *.o  *.ko *.mod.c  modules.order  Module.symvers
	rm -f .built-in.o.cmd 
	rm -f .coolrfreq.*
	rm -f .pstate_user.o.cmd .turbofreq.o.cmd
	rm -rf .tmp_versions


endif
