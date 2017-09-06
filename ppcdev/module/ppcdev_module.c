#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include "ppcdev_dev.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prophet Yang (prophet0321@gmail.com)");
MODULE_DESCRIPTION("Implement ppcdev");

static int __init ppcdev_module_init(void)
{
	int rc = 0;

	if ((rc = ppcdev_init()) != 0) {
		printk(KERN_ERR "[PPCDEV] ppcdev init failed: rc=%d", rc);
		goto ppcdev_init_failed;
	}

	return 0;

ppcdev_init_failed:
	ppcdev_release();

	return rc;
}

static void __exit ppcdev_module_cleanup(void)
{
	ppcdev_release();
}

module_init(ppcdev_module_init);
module_exit(ppcdev_module_cleanup);
