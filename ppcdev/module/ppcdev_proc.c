#include <linux/module.h>
#include <linux/proc_fs.h>
#include "ppcdev_proc.h"
#include "ppcdev_dev.h"

extern ppcdev_t ppcdev;

/* file operation functions */
ssize_t proc_ops_read_atomic_value (struct file *filp, char __user *buff, size_t count, loff_t *pos) {
	atomic_t *cnt = PDE_DATA(file_inode(filp));
	char tmpbuf[32] = {0};
	int len = 0;

	if (!cnt) {
		printk(KERN_DEBUG "Can not find proc data");
		return 0;
	}

	if (! access_ok(VERIFY_WRITE, buff, 32)) {
		printk(KERN_DEBUG "user buffer is invalid");
		return -EFAULT;
	}

	printk(KERN_DEBUG "%s(): count=%lu, pos=%lld", __func__, count, *pos);
	snprintf(tmpbuf, 32, "%d\n", atomic_read(cnt));
	len = strlen(tmpbuf);
	tmpbuf[len] = '\0';
	copy_to_user(buff, tmpbuf, strlen(tmpbuf));
	if (*pos == 0) {
		*pos += count;
	} else {
		return 0;
	}

	return len;
}

static struct file_operations pending_cnt_ops = {
	.read = proc_ops_read_atomic_value
};
/* proc_dir_entries */
static struct proc_dir_entry *ppcdev_proc_parent = NULL;
static struct proc_dir_entry *ppcdev_proc_queue_parent = NULL;
static struct proc_dir_entry *ppcdev_proc_queue_pending_cnt = NULL;
static struct proc_dir_entry *ppcdev_proc_queue_doing_cnt = NULL;


static void ppcdev_proc_remove_all(void) {
	if (ppcdev_proc_queue_doing_cnt) {
		proc_remove(ppcdev_proc_queue_doing_cnt);
		ppcdev_proc_queue_doing_cnt = NULL;
	}
	if (ppcdev_proc_queue_pending_cnt) {
		proc_remove(ppcdev_proc_queue_pending_cnt);
		ppcdev_proc_queue_pending_cnt = NULL;
	}
	if (ppcdev_proc_queue_parent) {
		proc_remove(ppcdev_proc_queue_parent);
		ppcdev_proc_queue_parent = NULL;
	}
	if (ppcdev_proc_parent) {
		proc_remove(ppcdev_proc_parent);
		ppcdev_proc_parent = NULL;
	}
}

int ppcdev_proc_init(void) {
	ppcdev_proc_parent = proc_mkdir("driver/ppcdev", NULL);
	if (! ppcdev_proc_parent) {
		goto proc_failed;
	}

	ppcdev_proc_queue_parent = proc_mkdir("queues", ppcdev_proc_parent);
	if (! ppcdev_proc_queue_parent) {
		goto proc_failed;
	}

	ppcdev_proc_queue_pending_cnt = proc_create_data("pending_count", 0444, ppcdev_proc_queue_parent, &pending_cnt_ops, &ppcdev.pending_queue.cnt);
	if (! ppcdev_proc_queue_pending_cnt) {
		goto proc_failed;
	}

	ppcdev_proc_queue_doing_cnt = proc_create_data("doing_count", 0444, ppcdev_proc_queue_parent, &pending_cnt_ops, &ppcdev.doing_queue.cnt);
	if (! ppcdev_proc_queue_pending_cnt) {
		goto proc_failed;
	}
	
	return 0;

proc_failed:
	ppcdev_proc_remove_all();
	return -ENOMEM;
}

void ppcdev_proc_fini(void) {
	ppcdev_proc_remove_all();
}
