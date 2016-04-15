#ifndef PPCDEV_H
#define PPCDEV_H

#include <linux/cdev.h>
#include <linux/atomic.h>

typedef struct ppcdev_s {
	atomic_t refcnt;
	struct cdev cdev;
} ppcdev_t;

extern ppcdev_t ppcdev;

#endif
