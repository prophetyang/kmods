#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include "ppcdev_ioctl.h"

#define MAX_EPOLL_EVENTS	8

int main(int argc, char **argv) {
	int rc = 0, devfd = 0, ioctl_test_num = 10;

	int epfd = 0, n_events = 0, i = 0;
	struct epoll_event ev, evlists[MAX_EPOLL_EVENTS];
	memset(&evlists, 0, sizeof(evlists));

	if ((devfd = open("/dev/ppcdev", O_RDWR)) < 0) {
		printf("Open device failed: rc=%d, errno=%d, %s\n", devfd, errno, strerror(errno));
		return -1;
	}
	printf("Open device /dev/ppcdev successfully.\n");

	if ((rc = ioctl(devfd, PPCDEV_IOCTL_CMD_GET_VALUE, &ioctl_test_num)) < 0) {
		printf("IOCTL failed: cmd=PPCDEV_IOCTL_CMD_GET_VALUE, rc=%d\n", rc);
		return rc;
	}

	ioctl_test_num = 32123;
	if ((rc = ioctl(devfd, PPCDEV_IOCTL_CMD_SET_VALUE, &ioctl_test_num)) < 0) {
		printf("IOCTL failed: cmd=PPCDEV_IOCTL_CMD_SET_VALUE, rc=%d\n", rc);
		return rc;
	}

	epfd = epoll_create(MAX_EPOLL_EVENTS);
	if (epfd == -1) {
		printf("epoll_create failed\n");
		return -1;
	}

	ev.events = EPOLLIN;
	ev.data.fd = devfd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, devfd, &ev) == -1) {
		printf("epoll_ctl failed\n");
		return -1;
	}

	while (1) {
		n_events = epoll_wait(epfd, evlists, MAX_EPOLL_EVENTS, 1000);

		for (i=0; i<n_events; i++) {
			if (evlists[i].events & EPOLLIN) {
				timer_event_t timer_event;
				timer_action_t timer_action;

#ifndef USE_IOCTL
				if (read(evlists[i].data.fd, &timer_event, sizeof(timer_event)) != sizeof(timer_event)) {
					printf("read failed");
					continue;
				}
				printf("read event id=%u, jiffies=%lu\n", timer_event.id, timer_event.jiffies);

				timer_action.id = timer_event.id;
				timer_action.action = TIMER_EVENT_ACTION_SET_1;
				write(evlists[i].data.fd, &timer_action, sizeof(timer_action));
				printf("write action id=%u, action=%d\n", timer_action.id, timer_action.action);
#else
				if ((rc = ioctl(devfd, PPCDEV_IOCTL_CMD_GET_EVENT, &timer_event)) < 0) {
					printf("IOCTL failed: cmd=PPCDEV_IOCTL_CMD_GET_EVENT, rc=%d\n", rc);
					continue;
				}
				printf("IOCTL read event id=%u, jiffies=%lu\n", timer_event.id, timer_event.jiffies);

				timer_action.id = timer_event.id;
				timer_action.action = TIMER_EVENT_ACTION_SET_1;
				if ((rc = ioctl(devfd, PPCDEV_IOCTL_CMD_SET_EVENT, &timer_action)) != 0) {
					printf("IOCTL failed: cmd=PPCDEV_IOCTL_CMD_SET_EVENT, rc=%d\n", rc);
					continue;
				}
				printf("IOCTL write action id=%u, action=%d\n", timer_action.id, timer_action.action);
#endif
			}
		}
	}

	close(devfd);
	printf("Close device.\n");
	return 0;
}
