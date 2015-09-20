/*
 * ioctl-based APIs for user-level pstate control for experiments
 *
 *
 * Contact: Kazutomo Yoshii <ky@anl.gov>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "coolrfreq.h"

static int pstate_user_major = 210;
static int pstate_user_minor;
static dev_t pstate_user_dev;
static struct cdev  pstate_user_cdev;

enum {
	PSTATE_INFO = 0,
	PSTATE_SET,
	PSTATE_DEBUG,
};

union  pstate_param {
	struct _info {
		u16 min, max, turbo, scaling;
	} info;
	struct _set {
		u16 request, old;
	} set;
};

static long pstate_user_ioctl(struct file *file,
			      unsigned int cmd,
			      unsigned long data)
{
	void __user *arg = (void __user *)data;
	union pstate_param param;
	int cpuid;
	u64 aperf, mperf;

	if (copy_from_user(&param, arg, sizeof(param)))
		return  -EFAULT;

	cpuid = smp_processor_id();

	switch (cmd) {
	case PSTATE_INFO:
		param.info.min =
			core_get_min_pstate_local();
		param.info.max =
			core_get_max_pstate_local();
		param.info.turbo =
			core_get_turbo_pstate_local();
		param.info.scaling =
			core_get_scaling_local();
		break;
	case PSTATE_SET:
		param.set.old =
			core_get_pstate_local();
		core_set_pstate_local(param.set.request);
		break;
	case PSTATE_DEBUG:
	default:
		rdmsrl(MSR_IA32_APERF, aperf);
		rdmsrl(MSR_IA32_MPERF, mperf);
		printk("cpuid=%d aperf=%llu mperf=%llu\n", cpuid, aperf, mperf);
		break;
	}

	if (copy_to_user(arg, &param, sizeof(param)))
		return -EFAULT;

	return 0;
}

static int pstate_user_release(struct inode *in, struct file *file)
{
	return 0;
}

static int  pstate_user_open(struct inode *in, struct file *file)
{
	return 0;
}


static const struct file_operations pstate_user_fops = {
	.owner = THIS_MODULE,
	.open = pstate_user_open,
	.release = pstate_user_release,
	.unlocked_ioctl = pstate_user_ioctl,
};


int init_pstate_user_dev(void)
{
	int rc;

	/* we need to check the cpu type here. e.g., tsc invariant,
	 * aperf availability
	 */

	pstate_user_dev = MKDEV(pstate_user_major, pstate_user_minor);

	rc = register_chrdev_region(pstate_user_dev, 1, "pstate_user");
	if (rc < 0) {
		printk("register_chrdev() failed. major=%d\n",  rc);
		return rc;
	}

	cdev_init(&pstate_user_cdev, &pstate_user_fops);
	rc = cdev_add(&pstate_user_cdev, pstate_user_dev, 1);
	if (rc) {
		printk("cdev_add() failed. rc=%d\n", rc);
		goto out_cdev;
	}

	return rc;

 out_cdev:
	unregister_chrdev_region(pstate_user_dev, 1);

	return rc;
}

void fini_pstate_user_dev(void)
{
	cdev_del(&pstate_user_cdev);

	unregister_chrdev_region(pstate_user_dev, 1);
}
