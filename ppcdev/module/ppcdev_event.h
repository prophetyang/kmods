#ifndef __PPCDEV_EVENT_H__
#define __PPCDEV_EVENT_H__

typedef enum {
	TIMER_EVENT_ACTION_NONE = 0,
	TIMER_EVENT_ACTION_SET_1,
	TIMER_EVENT_ACTION_SET_2,
	TIMER_EVENT_ACTION_MAX
} timer_event_actions_t;

typedef struct timer_action {
	unsigned int id;
	timer_event_actions_t action;
} timer_action_t;

typedef struct timer_event {
	unsigned int id;
	unsigned long jiffies;
} timer_event_t;

#ifdef __KERNEL__
#include <linux/list.h>
typedef struct ppcdev_event {
	timer_event_t evt;
	struct list_head link;
} ppcdev_event_t;

extern int ppcdev_event_init(void);
extern void ppcdev_event_fini(void);
extern ppcdev_event_t *ppcdev_event_alloc(void);
extern void ppcdev_event_free(ppcdev_event_t *event);
#endif // __KERNEL__

#endif
