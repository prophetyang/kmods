MAKE=$(shell which make)

obj-m = jprobe_sys_mount.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
