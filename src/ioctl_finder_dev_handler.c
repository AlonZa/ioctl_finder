#include "include/ioctl_finder_dev_handler.h"

ssize_t ioctl_finder_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	// char* path = (char*)kzalloc(len + 1, GFP_KERNEL);
	// if (path == NULL) {
	// 	return -ENOMEM;
	// }

	// /* read data from user buffer to my_data->buffer */
	// if (copy_from_user(path, buf, len))
	// 	return -EFAULT;

	// path[len - 1] = 0;

	// if (is_unlocked_ioctl_implemented(path)){
	// 	printk(KERN_INFO IOCTL_FINDER_PRNT_SUCCESS "Found unclocked_ioctl :)\n");
	// 	return -244;
	// }
	// else {
	// 	printk(KERN_INFO IOCTL_FINDER_PRNT_ERROR "No unclocked_ioctl here.\n");
	// 	return len;
	// }

	return 0;
}

bool is_unlocked_ioctl_implemented(struct ioctl_finder_req *req)
{
    struct file *o_file = filp_open(req->dev_path, 0, 0);
	struct file_operations *f_ops;

	if (!IS_ERR_OR_NULL(o_file)) {
		f_ops = o_file->f_op;
        printk("o_file OK");
    }
	else {
		return 0;
	}

	if (!IS_ERR_OR_NULL(f_ops)) {
        printk("f_ops OK");
        if (f_ops->unlocked_ioctl) {
            printk("f_ops->unlocked_ioctl OK");

            if (f_ops->owner) {
                strncpy((req->response).dev_owner_name, f_ops->owner->name, sizeof((req->response).dev_owner_name));
            }
            ((req)->response).unlocked_ioctl_p = (unsigned long)f_ops->unlocked_ioctl;

            filp_close(o_file, 0);
            return 1;
        }
        else {
            printk("f_ops->unlocked_ioctl NOT OK");
            filp_close(o_file, 0);
            return 0;
        }
    }
    
    return 0;
}

static long ioctl_finder_cmd_handler(struct ioctl_finder_req __user *u_req) {

	struct ioctl_finder_req *k_req = (struct ioctl_finder_req *)kzalloc(sizeof(struct ioctl_finder_req), GFP_KERNEL);
	int ret;

	if(!k_req) {
		return -ENOMEM;
	}

	ret = copy_from_user(k_req, u_req, sizeof(struct ioctl_finder_req));
	if (ret) {
		return -EFAULT;
	}

	if (is_unlocked_ioctl_implemented(k_req)){
		printk(KERN_INFO IOCTL_FINDER_PRNT_SUCCESS "Found unclocked_ioctl :)\n");
		ret = copy_to_user(u_req, k_req, sizeof(struct ioctl_finder_req));
		if (ret) {
			return -EFAULT;
		}
	}
	else {
		printk(KERN_INFO IOCTL_FINDER_PRNT_ERROR "No unclocked_ioctl here.\n");
	}

	return 0;
}

long ioctl_finder_ioctl_handler(struct file *filp, unsigned int cmd, unsigned long arg) {

	struct ioctl_finder_req __user *argp = (struct ioctl_finder_req *)arg;

	switch (cmd)
	{
	case IO_IOCTL_FINDER:
		return ioctl_finder_cmd_handler(argp);

	default:
		break;
	}

	return -1;

}
