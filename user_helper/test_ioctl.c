#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEVICE_FILENAME "/dev/ioctl_finder_dev"

#define IO_IOCTL_FINDER _IOWR('I', 0x44, struct ioctl_finder_req)

struct ioctl_finder_res {
    char dev_owner_name[128];
    unsigned long unlocked_ioctl_p;
};

struct ioctl_finder_req {
    char dev_path[128];
    struct ioctl_finder_res response;
};

int main() {
    int fd;
    struct ioctl_finder_req req;

    memset(&req, 0, sizeof(struct ioctl_finder_req));
    strcpy(req.dev_path, "/dev/vmci");
    
    fd = open(DEVICE_FILENAME, O_RDWR);
    if (fd < 0) {
        printf("Error opening device file: %s\n", DEVICE_FILENAME);
        return -1;
    }
    
    ioctl(fd, IO_IOCTL_FINDER, &req);

    printf("module: %s\n\tunlocked_ioctl: %lx\n", req.response.dev_owner_name, req.response.unlocked_ioctl_p);

    close(fd);
    return 0;
}
