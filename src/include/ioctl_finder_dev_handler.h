#ifndef _IOCTL_FINDER_DEV_HANDLER_
#define _IOCTL_FINDER_DEV_HANDLER_

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "utils.h"

struct ioctl_finder_res {
    char dev_owner_name[512];
    unsigned long unlocked_ioctl_p;
};

struct ioctl_finder_req {
    char dev_path[512];
    struct ioctl_finder_res response;
};

#define IO_IOCTL_FINDER _IOWR('I', 0x44, struct ioctl_finder_req)

ssize_t ioctl_finder_write(struct file *filp, const char *buf, size_t len, loff_t * off);
long ioctl_finder_ioctl_handler(struct file *filp, unsigned int cmd, unsigned long arg);
bool is_unlocked_ioctl_implemented(struct ioctl_finder_req *req);

#endif