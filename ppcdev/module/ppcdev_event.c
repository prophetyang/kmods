#include <linux/slab.h>
#include <linux/version.h>
#include <linux/timer.h>
#include "ppcdev_event.h"
#include "ppcdev_dev.h"

static struct kmem_cache *ppcdev_event_cache_pool = NULL;
static atomic_t ppcdev_event_id = ATOMIC_INIT(0);
static struct timer_list event_gen_timer;

static void event_gen_timer_callbck(unsigned long data) {
	ppcdev_add_event(jiffies);
	mod_timer(&event_gen_timer, jiffies + HZ);
}

ppcdev_event_t *ppcdev_event_alloc(void) {
	ppcdev_event_t *event = kmem_cache_zalloc(ppcdev_event_cache_pool, GFP_KERNEL);

	if (event) {
		event->evt.id = atomic_inc_return(&ppcdev_event_id);
	}

	return event;
}

void ppcdev_event_free(ppcdev_event_t *event) {
	if (event) {
		kmem_cache_free(ppcdev_event_cache_pool, event);
	}
}

int ppcdev_event_init(void) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23))
	ppcdev_event_cache_pool = kmem_cache_create("ppcdev_event_cache_pool", sizeof(ppcdev_event_t), 0, 0, NULL);
#else   
	ppcdev_event_cache_pool = kmem_cache_create("ppcdev_event_cache_pool", sizeof(ppcdev_event_t), 0, 0, NULL, NULL);
#endif
	if (! ppcdev_event_cache_pool)
		return -ENOMEM;

	init_timer(&event_gen_timer);
	event_gen_timer.expires = jiffies + HZ;
	event_gen_timer.function = &event_gen_timer_callbck;
	event_gen_timer.data = 0;
	add_timer(&event_gen_timer);

	return 0;
}

void ppcdev_event_fini(void) {
	del_timer(&event_gen_timer);

	if (ppcdev_event_cache_pool) {
		kmem_cache_destroy(ppcdev_event_cache_pool);
	}
}
