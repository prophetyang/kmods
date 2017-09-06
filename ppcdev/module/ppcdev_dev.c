#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include "ppcdev_dev.h"
#include "ppcdev_event.h"
#include "ppcdev_proc.h"
#include "ppcdev_fops.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prophet Yang (prophet0321@gmail.com)");
MODULE_DESCRIPTION("Implement ppcdev");

const static int ppcdev_device_minor = 0;
const static int ppcdev_max_devices = 1;

static int ppcdev_device_major = 0;
static struct class *ppcdev_class = NULL;
static struct device *ppcdev_device = NULL;
static dev_t dev_num = 0;

char *ppcdev_device_name = "ppcdev";
ppcdev_t ppcdev;

static int ppcdev_setup(ppcdev_t *dev, dev_t major) {
	cdev_init(&dev->cdev, &ppcdev_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &ppcdev_fops;

	return cdev_add(&dev->cdev, MKDEV(major, 0), 1);
}

static void ppcdev_cleanup(void) {
	ppcdev_queue_flush(&ppcdev.pending_queue);
	ppcdev_queue_flush(&ppcdev.doing_queue);

	ppcdev_event_fini();
	ppcdev_proc_fini();

	/* clear cdev first */
	cdev_del(&ppcdev.cdev);

	/* destroy device */
	if (ppcdev_device && ppcdev_class) {
		device_destroy(ppcdev_class, dev_num);
	}
	
	/* destroy class */
	if (ppcdev_class) {
		class_destroy(ppcdev_class);
	}

	/* unregister char device region */
	unregister_chrdev_region(dev_num, ppcdev_max_devices);

	printk(KERN_INFO "[PPCDEV] Cleaning up %s module.", ppcdev_device_name);
}

int ppcdev_add_event(unsigned long jiffies) {
	int rc = 0;
	ppcdev_event_t *event = ppcdev_event_alloc();;

	if (! event) {
		return -ENOMEM;
	}

	event->evt.jiffies = jiffies;

	if ((rc = ppcdev_enqueue(&ppcdev.pending_queue, event)) != 0) {
		ppcdev_event_free(event);
		return rc;
	}

	wake_up_interruptible(&ppcdev.wq);

	printk(KERN_DEBUG "%s(): id=%u, jiffies=%lu", __func__, event->evt.id, event->evt.jiffies);

	return 0;
}

ppcdev_event_t *ppcdev_get_event(void) {
	ppcdev_event_t *event = ppcdev_dequeue(&ppcdev.pending_queue);

	if (!event) {
		printk(KERN_DEBUG "%s(): no event", __func__);
		return NULL;
	}

	if (ppcdev_enqueue(&ppcdev.doing_queue, event) != 0) {
		printk(KERN_DEBUG "%s(): move to %s failed, id=%u, jiffies=%lu", __func__, ppcdev.doing_queue.name, event->evt.id, event->evt.jiffies);
		ppcdev_event_free(event);
		return NULL;
	}

	printk(KERN_DEBUG "%s(): id=%u, jiffies=%lu", __func__, event->evt.id, event->evt.jiffies);

	return event;
}

int ppcdev_set_event_action(timer_action_t *timer_action) {
	ppcdev_event_t *event = NULL;

	if (! timer_action)
		return -EINVAL;

	event = ppcdev_queue_search(&ppcdev.doing_queue, timer_action->id);
	if (! event) {
		printk(KERN_DEBUG "%s(): no such event in %s (id=%u)", __func__, ppcdev.doing_queue.name, timer_action->id);
		return -ENODATA; /* TODO: udpate error code */
	}

	printk(KERN_DEBUG "%s(): id=%u, action=%u", __func__, timer_action->id, timer_action->action);
	/* handle action here?? */
	ppcdev_queue_remove(&ppcdev.doing_queue, event);
	ppcdev_event_free(event);

	return 0;
}

int ppcdev_init(void)
{
	int rc = 0;

	if ((rc = ppcdev_event_init()) != 0) {
		goto ppcdev_event_init_failed;
	}

	/* Let kernel assign major number for us since we don't want to specify it in our device */
	if ((rc = alloc_chrdev_region(&dev_num, ppcdev_device_minor, ppcdev_max_devices, ppcdev_device_name)) < 0) {
		printk(KERN_ERR "[PPCDEV] Allocate major number for device %s failed: rc=%d", ppcdev_device_name, rc);
		goto alloc_chrcdev_region_failed;
	}
	
	/* get major number */
	ppcdev_device_major = MAJOR(dev_num);

	/* create /sys/class/ppcdev */
	ppcdev_class = class_create(THIS_MODULE, ppcdev_device_name);
	if (PTR_ERR_OR_ZERO(ppcdev_class)) {
		printk(KERN_ERR "[PPCDEV] Create class for device %s failed", ppcdev_device_name);
		rc = -EFAULT;
		goto class_create_failed;
	}

	/* create /dev/ppcdev */
	ppcdev_device = device_create(ppcdev_class, NULL, dev_num, NULL, ppcdev_device_name);
	if (PTR_ERR_OR_ZERO(ppcdev_device)) {
		printk(KERN_ERR "[PPCDEV] Create device for device %s failed", ppcdev_device_name);
		rc = -EFAULT;
		goto device_create_failed;
	}

	if ((rc = ppcdev_setup(&ppcdev, ppcdev_device_major)) < 0) {
		printk(KERN_ERR "[PPCDEV] device %s setup failed: rc=%d", ppcdev_device_name, rc);
		goto ppcdev_setup_failed;
	}

	if ((rc = ppcdev_proc_init()) != 0) {
		printk(KERN_ERR "[PPCDEV] proc init failed: rc=%d", rc);
		goto ppcdev_proc_init_failed;
	}

	init_waitqueue_head(&ppcdev.wq);
	ppcdev_queue_init(&ppcdev.pending_queue, "pending_queue");
	ppcdev_queue_init(&ppcdev.doing_queue, "doing_queue");

	printk(KERN_INFO "[PPCDEV] device /dev/%s is created successfully.",
			ppcdev_device_name);

	return 0;

ppcdev_proc_init_failed:
	ppcdev_proc_fini();

ppcdev_setup_failed:
	device_destroy(ppcdev_class, dev_num);

device_create_failed:
	class_destroy(ppcdev_class);

class_create_failed:
	unregister_chrdev_region(dev_num, ppcdev_max_devices);

alloc_chrcdev_region_failed:
	ppcdev_event_fini();

ppcdev_event_init_failed:

	return rc;
}

void ppcdev_release(void)
{
	ppcdev_cleanup();
}

