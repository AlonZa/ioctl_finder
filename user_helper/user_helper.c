#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

//------------------------------------------//
//                 defines                  //
//------------------------------------------//

#define DEVICE_FILENAME "/dev/ioctl_finder_dev"

#define IO_IOCTL_FINDER _IOWR('I', 0x44, struct ioctl_finder_req)

#define OUTPUT_SIZE  256

struct ioctl_finder_res {
    char dev_owner_name[512];
    unsigned long unlocked_ioctl_p;
    unsigned long compat_ioctl_p;
};

struct ioctl_finder_req {
    char dev_path[512];
    struct ioctl_finder_res response;
};

//------------------------------------------//

int filter(const struct dirent *ent) {
    if (ent->d_name[0] == '.') {
        return 0;
    }
    return 1;
}

int get_kernel_symbol(char *output, unsigned long ptr)
{
    char command[512] = {0};
    FILE *fp;

    if (ptr == 0) {
        return -1;
    }

    // The last 'cut' delimeter is a tab charachter (0x09 in ASCII).
    sprintf(command, "cat /proc/kallsyms | grep %lx | tr -s ' ' | cut -d\" \" -f3 | cut -d'\x09' -f1", ptr);

    fp = popen(command, "r");

    if (!fp) {
        return -1;
    }

    if (fgets(output, OUTPUT_SIZE - 1, fp) != NULL) {
        output[strlen(output) - 1] = 0;
        pclose(fp);
        return 0;
    }

    pclose(fp);

    return -1;
}

int main(int argc, char *argv[]) {
    struct dirent **namelist;
    int n;
    int fd = open(DEVICE_FILENAME, O_RDWR);
    struct ioctl_finder_req req;
    char *output_unlocked_command = calloc(OUTPUT_SIZE, sizeof(char));
    char *output_compat_ommand = calloc(OUTPUT_SIZE, sizeof(char));

    n = scandir("/dev", &namelist, filter, alphasort);
    if (n < 0) {
        perror("scandir");
    } else {
        while (n--) {
            memset(&req, 0, sizeof(struct ioctl_finder_req));
            snprintf(req.dev_path, sizeof(req.dev_path) - 1, "/dev/%s", namelist[n]->d_name);

            free(namelist[n]);

            ioctl(fd, IO_IOCTL_FINDER, &req);

            if (get_kernel_symbol(output_unlocked_command, req.response.unlocked_ioctl_p)) {
                snprintf(output_unlocked_command, 5, "None");
            }

            if (get_kernel_symbol(output_compat_ommand, req.response.compat_ioctl_p)) {
                snprintf(output_compat_ommand, 5, "None");
            }

            if (req.response.unlocked_ioctl_p) {
                printf("dev: %s\n\tmodule: %s\n\tunlocked_ioctl: %s (0x%lx)\n\tcompact_ioctl: %s (0x%lx)\n", 
                        req.dev_path,
                        req.response.dev_owner_name[0] ? req.response.dev_owner_name : "No owner module", 
                        output_unlocked_command,
                        req.response.unlocked_ioctl_p,
                        output_compat_ommand,
                        req.response.compat_ioctl_p);
            }

            memset(output_unlocked_command, 0, OUTPUT_SIZE);
            memset(output_compat_ommand, 0, OUTPUT_SIZE);

        }
        free(namelist);
    }

    close(fd);
    return 0;
}