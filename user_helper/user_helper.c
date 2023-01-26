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

struct ioctl_finder_res {
    char dev_owner_name[512];
    unsigned long unlocked_ioctl_p;
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

int main(int argc, char *argv[]) {
    struct dirent **namelist;
    int n;
    int fd = open(DEVICE_FILENAME, O_RDWR);
    struct ioctl_finder_req req;

    n = scandir("/dev", &namelist, filter, alphasort);
    if (n < 0) {
        perror("scandir");
    } else {
        while (n--) {
            // char f_path[128] = {0};
            // snprintf(f_path, 127, "/dev/%s", namelist[n]->d_name);

            memset(&req, 0, sizeof(struct ioctl_finder_req));
            snprintf(req.dev_path, sizeof(req.dev_path) - 1, "/dev/%s", namelist[n]->d_name);

            free(namelist[n]);

            ioctl(fd, IO_IOCTL_FINDER, &req);

            if (req.response.unlocked_ioctl_p) {
                printf("dev: %s\n\tmodule: %s\n\tunlocked_ioctl: %lx\n", 
                        req.dev_path,
                        req.response.dev_owner_name[0] ? req.response.dev_owner_name : "No owner module", 
                        req.response.unlocked_ioctl_p);
            }

            // int ret = write(fd, f_path, strlen(f_path) + 1);
            // if (ret < 0) {
            //     if (errno == 244) {
            //         printf("%s - IOCTL Found\n", f_path);
            //     }
            // }

        }
        free(namelist);
    }

    close(fd);
    return 0;
}