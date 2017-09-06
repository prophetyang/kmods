#include "ppcdev_queue.h"
#include "ppcdev_event.h"

int ppcdev_queue_remove(ppcdev_queue_t *q, ppcdev_event_t *event) {
	if (!q || !event)
		return -EINVAL;

	spin_lock_bh(&q->lock);
	list_del(&event->link);
	atomic_dec(&q->cnt);
	spin_unlock_bh(&q->lock);

	return 0;
}

ppcdev_event_t * ppcdev_queue_search(ppcdev_queue_t *q, unsigned int id) {
	struct list_head *pos;
	ppcdev_event_t *event;
	int matched = 0;

	if (!q)
		return NULL;

	spin_lock_bh(&q->lock);
	list_for_each(pos, &q->head) {
		event = list_entry(pos, ppcdev_event_t, link);
		if (event && (event->evt.id == id)) {
			matched = 1;
			break;
		}
	}
	spin_unlock_bh(&q->lock);

	return (matched) ? event : NULL;
}

int ppcdev_enqueue(ppcdev_queue_t *q, ppcdev_event_t *event) {
	if (!q || !event)
		return -EINVAL;

	spin_lock_bh(&q->lock);
	list_add_tail(&event->link, &q->head);
	atomic_inc(&q->cnt);
	spin_unlock_bh(&q->lock);

	return 0;
}

ppcdev_event_t * ppcdev_dequeue(ppcdev_queue_t *q) {
	ppcdev_event_t *event = NULL;

	if (!q)
		return NULL;

	spin_lock_bh(&q->lock);
	if (! list_empty(&q->head)) {
		event = list_first_entry(&q->head, ppcdev_event_t, link);
		list_del(&event->link);
		atomic_dec(&q->cnt);
	}
	spin_unlock_bh(&q->lock);

	return event;
}

int ppcdev_queue_empty(ppcdev_queue_t *q) {
	int ret = 0;

	if (! q)
		return 1;

	spin_lock_bh(&q->lock);
	ret = list_empty(&q->head);
	spin_unlock_bh(&q->lock);

	return ret;
}

void ppcdev_queue_flush(ppcdev_queue_t *q) {
	struct list_head *pos, *n;
	ppcdev_event_t *event = NULL;

	if (!q)
		return;

	spin_lock_bh(&q->lock);
	if (! list_empty(&q->head)) {
		list_for_each_safe(pos, n, &q->head) {
			event = list_entry(pos, ppcdev_event_t, link);
			list_del(&event->link);
			atomic_dec(&q->cnt);
			ppcdev_event_free(event);
		}
	}
	spin_unlock_bh(&q->lock);

	printk("[PPCDEV] %s(%s)", __func__, q->name);
}

int ppcdev_queue_init(ppcdev_queue_t *q, char *name) {
	if (!q)
		return -EINVAL;

	memset(q, 0, sizeof(ppcdev_queue_t));
	spin_lock_init(&q->lock);
	INIT_LIST_HEAD(&q->head);
	atomic_set(&q->cnt, 0);
	strncpy(q->name, name, strlen(name));

	return 0;
}

void ppcdev_queue_fini(void) {
}
