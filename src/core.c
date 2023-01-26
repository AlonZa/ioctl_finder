#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "include/utils.h"
#include "include/ioctl_finder_dev_handler.h"


dev_t ioctl_finder_dev = 0;
static struct class *ioctl_finder_dev_class;
static struct cdev ioctl_finder_cdev;

static struct file_operations ioctl_finder_fops =
{
    .owner      = THIS_MODULE,
    // .write      = ioctl_finder_write,
	.unlocked_ioctl = ioctl_finder_ioctl_handler,
};

static int ioctl_finder_init(void)
{

	printk(KERN_INFO IOCTL_FINDER_PRNT_INFO "ioctl_finder is up\n");

	/*Allocating Major number*/
	if((alloc_chrdev_region(&ioctl_finder_dev, 0, 1, IOCTL_FINDER_DEVICE_NAME)) <0){
			printk(KERN_INFO IOCTL_FINDER_PRNT_ERROR "Cannot allocate major number\n");
			return -1;
	}
	printk(KERN_INFO IOCTL_FINDER_PRNT_INFO "Major = %d Minor = %d \n",MAJOR(ioctl_finder_dev), MINOR(ioctl_finder_dev));

	/*Creating cdev structure*/
	cdev_init(&ioctl_finder_cdev,&ioctl_finder_fops);

	/*Adding character device to the system*/
	if((cdev_add(&ioctl_finder_cdev, ioctl_finder_dev, 1)) < 0){
		printk(KERN_INFO IOCTL_FINDER_PRNT_ERROR "Cannot add the device to the system\n");
		goto r_class;
	}

	/*Creating struct class*/
	if(IS_ERR(ioctl_finder_dev_class = class_create(THIS_MODULE, IOCTL_FINDER_CLASS_NAME))){
		printk(KERN_INFO IOCTL_FINDER_PRNT_ERROR "Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if(IS_ERR(device_create(ioctl_finder_dev_class, NULL, ioctl_finder_dev, NULL,IOCTL_FINDER_DEVICE_NAME))){
		printk(KERN_INFO IOCTL_FINDER_PRNT_ERROR "Cannot create the Device 1\n");
		goto r_device;
	}

	return 0;

r_device:
        class_destroy(ioctl_finder_dev_class);
r_class:
        unregister_chrdev_region(ioctl_finder_dev, 1);
        return -1;
}

static void ioctl_finder_clenup(void)
{
	device_destroy(ioctl_finder_dev_class, ioctl_finder_dev);
	class_destroy(ioctl_finder_dev_class);
	cdev_del(&ioctl_finder_cdev);
	unregister_chrdev_region(ioctl_finder_dev, 1);

	printk(KERN_INFO IOCTL_FINDER_PRNT_INFO "ioctl_finder is down!\n");
}

module_init(ioctl_finder_init);
module_exit(ioctl_finder_clenup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alon Zahavi");
MODULE_DESCRIPTION("A module that finds devices that implements `ioctl` function");