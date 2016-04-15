#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>

#include "ppcdev.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prophet Yang (prophet0321@gmail.com)");
MODULE_DESCRIPTION("Implement ppcdev");

const static char *ppcdev_device_name = "ppcdev";
const static int ppcdev_device_minor = 0;
const static int ppcdev_max_devices = 1;

static int ppcdev_device_major = 0;
static struct class *ppcdev_class = NULL;
static struct device *ppcdev_device = NULL;
static dev_t dev_num = 0;

ppcdev_t ppcdev;	

static int ppcdev_fops_open(struct inode *inode, struct file *filp) {
	ppcdev_t *dev = container_of(inode->i_cdev, ppcdev_t, cdev);
	if (dev) {
		int refcnt = 0;
		filp->private_data = dev;

		if ((refcnt = atomic_read(&dev->refcnt)) != 0) {
			printk(KERN_ERR "[PPDEV] open device %s failed, refcnt=%d\n", ppcdev_device_name, refcnt);
			return -EBUSY;
		}

		refcnt = atomic_inc_return(&dev->refcnt);
		printk(KERN_INFO "[PPDEV] device %s is opened, refcnt=%d\n", ppcdev_device_name, refcnt);
	}

	return 0;
}

static ssize_t ppcdev_fops_read(struct file *fp, char *buff, size_t length, loff_t *ppos) {
	return 0;
}

static ssize_t ppcdev_fops_write(struct file *fp, const char *buff, size_t length, loff_t *ppos) {
	return 0;
}

static int ppcdev_fops_release(struct inode *inode, struct file *filp) {
	ppcdev_t *dev = container_of(inode->i_cdev, ppcdev_t, cdev);
	if (dev) {
		int refcnt = atomic_dec_return(&dev->refcnt);
		printk(KERN_INFO "[PPDEV] device %s is released, refcnt=%d\n", ppcdev_device_name, refcnt);
	}

	return 0;
}

static struct file_operations ppcdev_fops = {
	.owner = THIS_MODULE,
	.open = ppcdev_fops_open,
	.release = ppcdev_fops_release,
	.read = ppcdev_fops_read,
	.write = ppcdev_fops_write
};

static int ppcdev_setup(ppcdev_t *dev, dev_t major) {
	cdev_init(&dev->cdev, &ppcdev_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &ppcdev_fops;

	return cdev_add(&dev->cdev, MKDEV(major, 0), 1);
}

static void ppcdev_cleanup(void) {
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

	printk(KERN_INFO "[PPDEV] Cleaning up %s module.\n", ppcdev_device_name);
}

static int __init ppcdev_module_init(void)
{
	int rc = 0;
	
	/* Let kernel assign major number for us since we don't want to specify it in our device */
	if ((rc = alloc_chrdev_region(&dev_num, ppcdev_device_minor, ppcdev_max_devices, ppcdev_device_name)) < 0) {
		printk(KERN_ERR "[PPCDEV] Allocate major number for device %s failed: rc=%d\n", ppcdev_device_name, rc);
		return rc;
	}
	
	/* get major number */
	ppcdev_device_major = MAJOR(dev_num);

	/* create /sys/class/ppcdev */
	ppcdev_class = class_create(THIS_MODULE, ppcdev_device_name);
	if (PTR_ERR_OR_ZERO(ppcdev_class)) {
		unregister_chrdev_region(dev_num, ppcdev_max_devices);
		printk(KERN_ERR "[PPDEV] Create class for device %s failed\n", ppcdev_device_name);
		return -EFAULT;
	}

	/* create /dev/ppcdev */
	ppcdev_device = device_create(ppcdev_class, NULL, dev_num, NULL, ppcdev_device_name);
	if (PTR_ERR_OR_ZERO(ppcdev_device)) {
		class_destroy(ppcdev_class); 
		unregister_chrdev_region(dev_num, ppcdev_max_devices);
		printk(KERN_ERR "[PPDEV] Create device for device %s failed\n", ppcdev_device_name);
		return -EFAULT;
	}

	if ((rc = ppcdev_setup(&ppcdev, ppcdev_device_major)) < 0) {
		printk(KERN_ERR "[PPDEV] device %s setup failed: rc=%d\n", ppcdev_device_name, rc);
		return rc;
	}

	printk(KERN_INFO "[PPDEV] device /dev/%s is created successfully.\n", 
			ppcdev_device_name);

	return 0;
}

static void __exit ppcdev_module_cleanup(void)
{
	ppcdev_cleanup();
}

module_init(ppcdev_module_init);
module_exit(ppcdev_module_cleanup);
