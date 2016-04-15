CC		= $(shell which gcc)
RM		= $(shell which rm)
CP		= $(shell which cp)

MODULE_DIR	= ../module
CFLAGS		= -I$(MODULE_DIR) -g

PPCDEV_USER	= ppcdev_user
PPCDEV_USER_OBJS	= ppcdev_user.o
OUTPUT_DIR	= ../output

all: $(PPCDEV_USER) install

$(PPCDEV_USER): $(PPCDEV_USER_OBJS)
	$(CC) $(CFLAGS) $(PPCDEV_USER_OBJS) -o $@

install: $(PPCDEV_USER)
	$(CP) -f $< $(OUTPUT_DIR)	

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -f *.o $(PPCDEV_USER)
