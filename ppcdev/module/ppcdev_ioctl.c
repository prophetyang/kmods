#include "ppcdev_queue.h"
#include "ppcdev_event.h"
#include "ppcdev_ioctl.h"
#include "ppcdev_dev.h"
#include <linux/capability.h>

int ppcdev_ioctl_num = 65536;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 0))
long ppcdev_ioctl( struct file *file, unsigned int cmd, unsigned long args) {
#else
int ppcdev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long args) {
#endif
	int ret = 0, err = 0;

	if (_IOC_TYPE(cmd) != PPCDEV_IOCTL_MAGIC) {
		printk(KERN_DEBUG "[PPCDEV] Invalid ioctl magic %d", _IOC_TYPE(cmd));
		return -ENOTTY;
	}

	if (_IOC_NR(cmd) > PPCDEV_IOCTL_CMD_INDEX_MAX) {
		printk(KERN_DEBUG "[PPCDEV] Invalid ioctl command index %d", _IOC_NR(cmd));
		return -ENOTTY;
	}

	if (_IOC_DIR(cmd) & _IOC_READ) {
		err = !access_ok(VERIFY_WRITE, (void __user*)args, _IOC_SIZE(cmd));
	} else if (_IOC_DIR(cmd) & _IOC_WRITE) {
		err = !access_ok(VERIFY_READ, (void __user *)args, _IOC_SIZE(cmd));
	}

	if (err) {
		return -EFAULT;
	}

	switch (cmd) {
		case PPCDEV_IOCTL_CMD_GET_VALUE:
			ret = __put_user(ppcdev_ioctl_num, (int __user *)args);
			printk(KERN_INFO "[PPCDEV] ioctl get value, ppcdev_ioctl_num=%d", ppcdev_ioctl_num);
			break;
		case PPCDEV_IOCTL_CMD_SET_VALUE:
			ret = __get_user(ppcdev_ioctl_num, (int __user *)args);
			printk(KERN_INFO "[PPCDEV] ioctl set value, ppcdev_ioctl_num=%d", ppcdev_ioctl_num);
			break;
		case PPCDEV_IOCTL_CMD_GET_EVENT:
			{
				ppcdev_event_t *event = NULL;

				if (! capable(CAP_SYS_ADMIN)) {
					return -EPERM;
				}

				event = ppcdev_get_event();
				if (event) {
					ret = copy_to_user((timer_event_t __user *)args, &event->evt, sizeof(timer_event_t));
					if (ret != 0) {
						/* re-enqueue the event due to get event failed */
						ppcdev_enqueue(&ppcdev.pending_queue, event);
						ret = -EFAULT;
						printk(KERN_ERR "[PPCDEV] ioctl copy_to_user() failed");
					}
				} else {
					printk(KERN_ERR "[PPCDEV] No event");
					/* hence: ret is 0 */
				}
			}
			break;
		case PPCDEV_IOCTL_CMD_SET_EVENT:
			{
				timer_action_t timer_action;

				if (! capable(CAP_SYS_ADMIN)) {
					return -EPERM;
				}

				memset(&timer_action, 0, sizeof(timer_action_t));

				ret = copy_from_user(&timer_action, (timer_action_t __user *)args, sizeof(timer_action_t));
				if (ret == 0) {
					ret = ppcdev_set_event_action(&timer_action);
				} else {
					ret = -EFAULT;
					printk(KERN_ERR "[PPCDEV] ioctl copy_from_user() failed");
				}
			}
			break;
		case PPCDEV_IOCTL_CMD_EXCHANGE_VALUE:
			break;
		default:
			ret = -ENOTTY;
	}

	return ret;
}
