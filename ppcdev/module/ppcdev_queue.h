#ifndef __PPCDEV_QUEUE_H__
#define __PPCDEV_QUEUE_H__

#ifdef __KERNEL__

#include <linux/spinlock.h>
#include <linux/list.h>
#include "ppcdev_event.h"

typedef struct {
	char name[64];
	spinlock_t lock;
	struct list_head head;
	atomic_t cnt;
} ppcdev_queue_t;

extern int ppcdev_queue_remove(ppcdev_queue_t *q, ppcdev_event_t *event);
extern ppcdev_event_t * ppcdev_queue_search(ppcdev_queue_t *q, unsigned int id);
extern int ppcdev_enqueue(ppcdev_queue_t *q, ppcdev_event_t *event);
extern ppcdev_event_t * ppcdev_dequeue(ppcdev_queue_t *q);
extern int ppcdev_queue_empty(ppcdev_queue_t *q);
extern void ppcdev_queue_flush(ppcdev_queue_t *q);
extern int ppcdev_queue_init(ppcdev_queue_t *q, char *name);
extern void ppcdev_queue_fini(void);

#endif // __KERNEL__

#endif
