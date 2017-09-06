#include <linux/poll.h>
#include "ppcdev_dev.h"
#include "ppcdev_fops.h"
#include "ppcdev_queue.h"

extern ppcdev_t ppcdev;

static int ppcdev_fops_open(struct inode *inode, struct file *filp) {
	ppcdev_t *dev = container_of(inode->i_cdev, ppcdev_t, cdev);
	if (dev) {
		int refcnt = 0;
		filp->private_data = dev;

		if ((refcnt = atomic_read(&dev->refcnt)) != 0) {
			printk(KERN_ERR "[PPCDEV] open device %s failed, refcnt=%d", ppcdev_device_name, refcnt);
			return -EBUSY;
		}

		refcnt = atomic_inc_return(&dev->refcnt);
		printk(KERN_INFO "[PPCDEV] device %s is opened, refcnt=%d", ppcdev_device_name, refcnt);
	}

	return 0;
}

static ssize_t ppcdev_fops_read(struct file *fp, char __user *buff, size_t length, loff_t *ppos) {
	ppcdev_event_t *event = NULL;
	int event_size = sizeof(timer_event_t);

	if (! access_ok(VERIFY_WRITE, buff, event_size)) {
		printk(KERN_ERR "%s(): insufficient buffer space", __func__);
		return -EFAULT;
	}

	event = ppcdev_dequeue(&ppcdev.pending_queue);
	if (!event) {
		printk(KERN_ERR "%s(): no event found", __func__);
		return -ENODATA;
	}

	if (copy_to_user(buff, &event->evt, sizeof(timer_event_t)) != 0) {
		/* copy_to_user() should complete at once in this driver, so if this fails,
		   we try to quene this event to pending queue.
		*/
		printk(KERN_ERR "%s(): copy_to_user() failed", __func__);
		if (ppcdev_enqueue(&ppcdev.pending_queue, event) != 0) {
			/* If enqueue fails, delete this event anyway :~ */
			printk(KERN_ERR "%s(): queue to pending queue failed, delete event id=%u, jiffies=%lu",
					__func__, event->evt.id, event->evt.jiffies);
			ppcdev_event_free(event);
		}

		return -EFAULT;
	}

	/* Enqueue the event to doing queue. If fails, delete the event anyway. */
	if (ppcdev_enqueue(&ppcdev.doing_queue, event) != 0) {
		printk(KERN_ERR "%s(): queue to doing queue failed, delete event id=%u, jiffies=%lu",
				__func__, event->evt.id, event->evt.jiffies);
		ppcdev_event_free(event);
		return -EFAULT;
	}

	length -= event_size;
	*ppos += event_size;;

	return event_size;
}

static ssize_t ppcdev_fops_write(struct file *fp, const char __user *buff, size_t length, loff_t *ppos) {
	ppcdev_event_t *event = NULL;
	timer_action_t timer_action;
	int event_size = sizeof(timer_action_t);

	if (! access_ok(VERIFY_READ, buff, event_size)) {
		printk(KERN_ERR "%s(): insufficient buffer space", __func__);
		return -EFAULT;
	}

	if (copy_from_user(&timer_action, buff, event_size) != 0) {
		printk(KERN_ERR "%s(): copy_from_user failed", __func__);
		return -EFAULT;
	}

	/* search if the event is in doing queue */
	event = ppcdev_queue_search(&ppcdev.doing_queue, timer_action.id);
	if (! event) {
		printk(KERN_ERR "%s(): No such event id=%u", __func__, timer_action.id);
		return -ENODATA;
	}

	/* TODO: handle action here */

	/* remove from doing queue once action is handled */
	ppcdev_queue_remove(&ppcdev.doing_queue, event);
	/* free the event */
	ppcdev_event_free(event);

	length -= event_size;
	*ppos += event_size;

	return event_size;
}

static unsigned int ppcdev_fops_poll(struct file *filp, poll_table *wait) {
	ppcdev_t *dev = filp->private_data;

	if (! dev) {
		return -ENODEV;
	}

	poll_wait(filp, &dev->wq, wait);

	if (! ppcdev_queue_empty(&ppcdev.pending_queue)) {
		return POLLIN | POLLRDNORM;
	}

	return 0;
}

static int ppcdev_fops_release(struct inode *inode, struct file *filp) {
	ppcdev_t *dev = container_of(inode->i_cdev, ppcdev_t, cdev);
	if (dev) {
		int refcnt = atomic_dec_return(&dev->refcnt);
		printk(KERN_INFO "[PPCDEV] device %s is released, refcnt=%d", ppcdev_device_name, refcnt);
	}

	return 0;
}

struct file_operations ppcdev_fops = {
	.owner = THIS_MODULE,
	.open = ppcdev_fops_open,
	.release = ppcdev_fops_release,
	.read = ppcdev_fops_read,
	.write = ppcdev_fops_write,
	.poll = ppcdev_fops_poll,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 0)) 
	.unlocked_ioctl = ppcdev_ioctl,
#else
	.ioctl = ppcdev_ioctl,
#endif
};
