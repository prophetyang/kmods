MAKE=$(shell which make)

obj-m = plist_kthread.o

all: clean
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules	

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
