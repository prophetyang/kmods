#ifndef __PPCDEV_DEVICE_H__
#define __PPCDEV_DEVICE_H__


#ifdef __KERNEL__
#include <linux/cdev.h>
#include <linux/atomic.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include "ppcdev_event.h"
#include "ppcdev_queue.h"
#include "ppcdev_ioctl.h"

typedef struct ppcdev_s {
	atomic_t refcnt;
	struct cdev cdev;

	ppcdev_queue_t pending_queue;
	ppcdev_queue_t doing_queue;

	wait_queue_head_t wq;	/* for polling */
} ppcdev_t;

extern ppcdev_t ppcdev;
extern char *ppcdev_device_name;

extern int ppcdev_init(void);
extern void ppcdev_release(void);
extern int ppcdev_add_event(unsigned long jiffies);
extern ppcdev_event_t *ppcdev_get_event(void);
extern int ppcdev_set_event_action(timer_action_t *timer_action);

#endif // __KERNEL__

#endif // __PPCDEV_DEVICE_H__
