#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

#include "ppcdev.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prophet Yang (prophet0321@gmail.com)");
MODULE_DESCRIPTION("Implement ppcdev");

const static char *device_name = "ppcdev";
const static int dev_minor_num = 0;
const static int dev_elements = 1;
static int dev_major_num = 0;
dev_t dev_num;

ppcdev_t ppcdev;

static int ppcdev_fops_open(struct inode *inode, struct file *filp) {
	return 0;
}

static ssize_t ppcdev_fops_read(struct file *fp, char *buff, size_t length, loff_t *ppos) {
	return 0;
}

static ssize_t ppcdev_fops_write(struct file *fp, const char *buff, size_t length, loff_t *ppos) {
	return 0;
}

static int ppcdev_fops_release(struct inode *inode, struct file *filp) {
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

	return cdev_add(&dev->cdev, major, 1);
}

static int __init ppcdev_init(void)
{
	int rc = 0;
	
	/* Let kernel assign major number for us since we don't want to specify it in our device */
	if ((rc = alloc_chrdev_region(&dev_num, dev_minor_num, dev_elements, device_name)) < 0) {
		printk(KERN_ERR "Allocate major number for device %s failed: rc=%d\n", device_name, rc);
		return rc;
	}
	
	/* get major number */
	dev_major_num = MAJOR(dev_num);

	printk(KERN_INFO "[PPDEV] device %s is created successfully. (run mknod /dev/%s c %d %d)\n", 
			device_name, device_name, dev_major_num, dev_minor_num);
	
	/* TODO: use class_create() to create ppcdev in /sys/class */
	/* TODO: use device_create() to create ppcdev /dev */

	if ((rc = ppcdev_setup(&ppcdev, dev_major_num)) < 0) {
		printk(KERN_ERR "device %s setup failed: rc=%d\n", device_name, rc);
		return rc;
	}

	return 0;
}

static void __exit ppcdev_cleanup(void)
{
	cdev_del(&ppcdev.cdev);
	unregister_chrdev_region(dev_num, dev_elements);
	printk(KERN_INFO "Cleaning up %s module.\n", device_name);
}

module_init(ppcdev_init);
module_exit(ppcdev_cleanup);
