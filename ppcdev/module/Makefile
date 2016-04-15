MAKE	= $(shell which make)
MKDIR	= $(shell which mkdir)
RM	= $(shell which rm)
CP	= $(shell which cp)

# disable module signature checking #
CONFIG_MODULE_SIG=n

# define __KERNEL__
MODCFLAGS += -D__KERNEL__

OUTPUT_DIR = ../output
MODULE_NAME = ppcdev

obj-m = $(MODULE_NAME).o

all: clean $(MODULE_NAME) install

$(MODULE_NAME):
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install: $(MODULE_NAME)
	$(CP) -f $(MODULE_NAME).ko $(OUTPUT_DIR)

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 
