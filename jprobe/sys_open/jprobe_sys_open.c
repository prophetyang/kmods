#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prophet Yang (prophet0321@gmail.com)");
MODULE_DESCRIPTION("JProbe sys_open Kernel Module");

static struct jprobe jprobe_sys_open;

asmlinkage long jprobe_sys_open_handler(const char __user *filename, int flags, umode_t mode) {
	char *name = vmalloc(PAGE_SIZE);
	if (name) {
		memset(name, 0, PAGE_SIZE);
		strncpy(name, filename, strlen(filename));
		printk(KERN_INFO "%s(): sys_open(filename=%s, flags=%d, mode=%u)", __func__, name, flags, mode);
		vfree(name);
	}

	jprobe_return();
}


static int __init jprobe_sys_open_init(void)
{
	jprobe_sys_open.kp.symbol_name = "sys_open";
	jprobe_sys_open.entry = (kprobe_opcode_t *) jprobe_sys_open_handler;
	register_jprobe(&jprobe_sys_open);
	printk(KERN_INFO "Register %s module\n", module_name(THIS_MODULE));
	return 0;
}

static void __exit jprobe_sys_open_cleanup(void)
{
	unregister_jprobe(&jprobe_sys_open);
	printk(KERN_INFO "Cleaning up %s module.\n", module_name(THIS_MODULE));
}

module_init(jprobe_sys_open_init);
module_exit(jprobe_sys_open_cleanup);
