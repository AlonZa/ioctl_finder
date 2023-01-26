#include "include/utils.h"

// bool is_unlocked_ioctl_implemented(const char *f_path)
// {
//     struct file *o_file = filp_open(f_path, 0, 0);
// 	struct file_operations *f_ops;

//     printk(f_path);

// 	if (!IS_ERR_OR_NULL(o_file)) {
// 		f_ops = o_file->f_op;
//         printk("o_file OK");
//     }
// 	else {
// 		return 0;
// 	}

// 	if (!IS_ERR_OR_NULL(f_ops)) {
//         printk("f_ops OK");
//         if (f_ops->unlocked_ioctl) {
//             printk("f_ops->unlocked_ioctl OK");
// 		    printk("module owner: %s", f_ops->owner ? f_ops->owner->name : "None");
//             filp_close(o_file, 0);
//             return 1;
//         }
//         else {
//             printk("f_ops->unlocked_ioctl NOT OK");
//             filp_close(o_file, 0);
//             return 0;
//         }
//     }
    
//     return 0;
// }