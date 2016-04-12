#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You-Hsin Yang (prophet0321@gmail.com)");
MODULE_DESCRIPTION("A kernel module that dumps processes per second");

static uint64_t g_runtime = 0;
static struct task_struct *plist_task = NULL;

static int plist_func(void *data) {
	struct task_struct *task;

	while (1) {
		if (kthread_should_stop()) {
			break;
		}

		for_each_process(task) {
			if (task) {
				printk(KERN_DEBUG "[%d] ppid=%d command=%s, utime=%lu, stime=%lu, gtime=%lu\n", 
						task_pid_nr(task), 
						task_ppid_nr(task), 
						task->comm, 
						task->utime, task->stime, task->gtime);
			}
		}
		schedule_timeout(HZ);
	}
	
	printk(KERN_DEBUG "%s exits", plist_task->comm);

	return 0;
}

static int __init plist_init(void)
{
	int err = 0;
	plist_task = kthread_create(plist_func, &g_runtime, "plist_task");
	
	if (IS_ERR(plist_task)) {
		printk("Unable to start kernel thread.\n");
      	err = PTR_ERR(plist_task);
      	return err;
	}
	
	printk(KERN_DEBUG "plist_task is created, pid=%d", task_pid_nr(plist_task));
	wake_up_process(plist_task);	

	return 0;
}

static void __exit plist_cleanup(void)
{
	if (plist_task) {
		kthread_stop(plist_task);
		plist_task = NULL;
	}	

	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(plist_init);
module_exit(plist_cleanup);
