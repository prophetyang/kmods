#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "ppcdev.h"

int main(int argc, char **argv) {
	int rc = 0, fd = 0;

	if ((fd = open("/dev/ppcdev", O_RDWR)) < 0) {
		printf("Open device failed: rc=%d, errno=%d, %s\n", fd, errno, strerror(errno));	
		return -1;
	}
	printf("Open device /dev/ppcdev successfully.\n");

	sleep(20);

	close(fd);
	printf("Close device.\n");
	return 0;
}
