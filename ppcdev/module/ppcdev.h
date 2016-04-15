#ifndef PPCDEV_H
#define PPCDEV_H

#ifdef __KERNEL__
#include <linux/cdev.h>
#include <linux/atomic.h>

typedef struct ppcdev_s {
	atomic_t refcnt;
	struct cdev cdev;
} ppcdev_t;

extern ppcdev_t ppcdev;
#endif // __KERNEL__

#endif // PPCDEV_H
