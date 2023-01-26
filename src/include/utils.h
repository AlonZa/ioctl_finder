#ifndef _IOCTL_FINDER_UTILS_
#define _IOCTL_FINDER_UTILS_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/file.h>
#include <linux/namei.h>
#include <linux/slab.h>

#include "ioctl_finder_dev_handler.h"

#define IOCTL_FINDER_SUCCESS 	0
#define IOCTL_FINDER_FAIL 	-1

#define IOCTL_FINDER_DEVICE_NAME "ioctl_finder_dev"
#define IOCTL_FINDER_CLASS_NAME "ioctl_finder_class"

#define IOCTL_FINDER_PRNT 	"IOCTL_FINDER: "

#define IOCTL_FINDER_PRNT_SUCCESS IOCTL_FINDER_PRNT "[+] " 
#define IOCTL_FINDER_PRNT_ERROR	IOCTL_FINDER_PRNT "[-] " 
#define IOCTL_FINDER_PRNT_INFO	IOCTL_FINDER_PRNT "[*] "

// bool is_unlocked_ioctl_implemented(const char *f_path);

#endif
