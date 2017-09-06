#ifndef __PPCDEV_FILE_OPERATIONS_H__
#define __PPCDEV_FILE_OPERATIONS_H__

#ifdef __KERNEL__
#include <linux/fs.h>

extern struct file_operations ppcdev_fops;
#endif

#endif
