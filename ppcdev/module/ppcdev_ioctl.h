#ifndef __PPCDEV_IOCTL_H__
#define __PPCDEV_IOCTL_H__

#include "ppcdev_event.h"

#define PPCDEV_IOCTL_MAGIC	'p'

typedef enum PPCDEV_IOCTL_CMD_INDEX {
	PPCDEV_IOCTL_CMD_INDEX_GET_VALUE = 1,
	PPCDEV_IOCTL_CMD_INDEX_SET_VALUE,
	PPCDEV_IOCTL_CMD_INDEX_GET_EVENT,
	PPCDEV_IOCTL_CMD_INDEX_SET_EVENT,
	PPCDEV_IOCTL_CMD_INDEX_EXCHANGE_VALUE,
	PPCDEV_IOCTL_CMD_INDEX_MAX
} PPCDEV_IOCTL_CMD_INDEX;

#define PPCDEV_IOCTL_CMD_GET_VALUE	_IOR(PPCDEV_IOCTL_MAGIC, PPCDEV_IOCTL_CMD_INDEX_GET_VALUE, int)
#define PPCDEV_IOCTL_CMD_SET_VALUE	_IOW(PPCDEV_IOCTL_MAGIC, PPCDEV_IOCTL_CMD_INDEX_SET_VALUE, int)
#define PPCDEV_IOCTL_CMD_GET_EVENT	_IOR(PPCDEV_IOCTL_MAGIC, PPCDEV_IOCTL_CMD_INDEX_GET_EVENT, timer_event_t)
#define PPCDEV_IOCTL_CMD_SET_EVENT	_IOW(PPCDEV_IOCTL_MAGIC, PPCDEV_IOCTL_CMD_INDEX_SET_EVENT, timer_event_t)
#define PPCDEV_IOCTL_CMD_EXCHANGE_VALUE	_IOWR(PPCDEV_IOCTL_MAGIC, PPCDEV_IOCTL_CMD_INDEX_EXCHANGE_VALUE, int)

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

extern int ppcdev_ioctl_num;;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 0))
extern long ppcdev_ioctl(struct file *file, unsigned int cmd, unsigned long args);
#else
extern int ppcdev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long args);
#endif

#endif

#endif
