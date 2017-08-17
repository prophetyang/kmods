#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prophet Yang (prophet0321@gmail.com)");
MODULE_DESCRIPTION("JProbe sys_mount Kernel Module");

static struct jprobe jprobe_sys_mount;

#include <linux/string.h>
int copy_mount_string(const void __user *data, char **where)
{
	char *tmp;

	if (!data) {
		*where = NULL;
		return 0;
	}

	tmp = strndup_user(data, PAGE_SIZE);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	*where = tmp;
	return 0;
}

static long exact_copy_from_user(void *to, const void __user * from,
				 unsigned long n)
{
	char *t = to;
	const char __user *f = from;
	char c;

	if (!access_ok(VERIFY_READ, from, n))
		return n;

	while (n) {
		if (__get_user(c, f)) {
			memset(t, 0, n);
			break;
		}
		*t++ = c;
		f++;
		n--;
	}
	return n;
}

int copy_mount_options(const void __user * data, unsigned long *where)
{
	int i;
	unsigned long page;
	unsigned long size;

	*where = 0;
	if (!data)
		return 0;

	if (!(page = __get_free_page(GFP_KERNEL)))
		return -ENOMEM;

	/* We only care that *some* data at the address the user
	 * gave us is valid.  Just in case, we'll zero
	 * the remainder of the page.
	 */
	/* copy_from_user cannot cross TASK_SIZE ! */
	size = TASK_SIZE - (unsigned long)data;
	if (size > PAGE_SIZE)
		size = PAGE_SIZE;

	i = size - exact_copy_from_user((void *)page, data, size);
	if (!i) {
		free_page(page);
		return -EFAULT;
	}
	if (i != PAGE_SIZE)
		memset((char *)page + i, 0, PAGE_SIZE - i);
	*where = page;
	return 0;
}

asmlinkage long jprobe_sys_mount_handler(char __user *dev_name, char __user *dir_name,
				char __user *type, unsigned long flags,
				void __user *data)
{
	int ret = 0;
	char *kernel_type;
	char *kernel_dir;
	char *kernel_dev;
	unsigned long data_page;

	ret = copy_mount_string(type, &kernel_type);
	if (ret < 0)
		goto out_type;

	ret = copy_mount_string(dir_name, &kernel_dir);
	if (ret < 0)
		goto out_dir;

	ret = copy_mount_string(dev_name, &kernel_dev);
	if (ret < 0)
		goto out_dev;

	ret = copy_mount_options(data, &data_page);
	if (ret < 0)
		goto out_data;

	printk(KERN_INFO "%s() sys_mount(dev=%s, dir=%s, type=%s, flags=%lu)", __func__, kernel_dev, kernel_dir, kernel_type, flags);

	free_page(data_page);
out_data:
	kfree(kernel_dev);
out_dev:
	kfree(kernel_dir);
out_dir:
	kfree(kernel_type);
out_type:

	jprobe_return();
}

static int __init jprobe_sys_mount_init(void)
{
	jprobe_sys_mount.kp.symbol_name = "sys_mount";
	jprobe_sys_mount.entry = (kprobe_opcode_t *) jprobe_sys_mount_handler;
	register_jprobe(&jprobe_sys_mount);
	printk(KERN_INFO "Register %s module\n", module_name(THIS_MODULE));
	return 0;
}

static void __exit jprobe_sys_mount_cleanup(void)
{
	unregister_jprobe(&jprobe_sys_mount);
	printk(KERN_INFO "Cleaning up %s module.\n", module_name(THIS_MODULE));
}

module_init(jprobe_sys_mount_init);
module_exit(jprobe_sys_mount_cleanup);
