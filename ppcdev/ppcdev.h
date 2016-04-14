#ifndef PPCDEV_H
#define PPCDEV_H

#include <linux/cdev.h>

typedef struct ppcdev_s {
	struct cdev cdev;
} ppcdev_t;

extern ppcdev_t ppcdev;

#endif
