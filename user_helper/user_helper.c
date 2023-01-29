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
#include <dirent.h>
#include <regex.h>


//------------------------------------------//
//                 defines                  //
//------------------------------------------//

#define DEVICE_FILENAME "/dev/ioctl_finder_dev"

#define IO_IOCTL_FINDER _IOWR('I', 0x44, struct ioctl_finder_req)

#define OUTPUT_SIZE  512

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

int find_ioctl_symbol_src(char *output, const char *symbol, const char *kernel_src)
{
    // grep -nH -E "^long tty_ioctl\(struct file.*,*unsigned int.*,*unsigned long.*\)[^;]*$" -r /home/user/Documents/linux | cut -d: -f1,2

    char command[512] = {0};
    FILE *fp;

    if (!symbol || !kernel_src) {
        return -1;
    }

    snprintf(command, 511, "grep -E -n -o \"^.* long %s\\(\\)*\" -r %s | cut -d: -f1,2", symbol, kernel_src);

    fp = popen(command, "r");

    if (!fp) {
        return -1;
    }

    if (fgets(output, OUTPUT_SIZE - 1, fp) != NULL) {
        output[strlen(output) - 1] = 0;
        pclose(fp);
        return 0;
    }
    else {
        memset(command, 0, 512);
        snprintf(command, 511, "grep -E -n -o \"^(static )?.*long %s\\(struct file.*)[^;]*$\" -r %s | cut -d: -f1,2", symbol, kernel_src);
        fp = popen(command, "r");
        
        if (!fp) {
            return -1;
        }

        if (fgets(output, OUTPUT_SIZE - 1, fp) != NULL) {
            output[strlen(output) - 1] = 0;
            pclose(fp);
            return 0;
        }
    }

    pclose(fp);

    return -1;

}

int main(int argc, char *argv[]) {

    char *kernel_src;
    struct dirent **namelist;
    int n;
    struct ioctl_finder_req req;
    int fd;
    char *output_unlocked_symbol;
    char *output_compat_symbol;
    char *unlocked_symbol_src = NULL;
    char *compat_symbol_src = NULL;

    if (argc == 1) {
        kernel_src = NULL;
    }
    else if (argc == 2) {
        kernel_src = calloc(OUTPUT_SIZE, sizeof(char));
        snprintf(kernel_src, OUTPUT_SIZE - 1, argv[1]);
    }
    else {
        puts("Usage:\n\t./ioctl_finder_user_helper\n\tor\n\t./ioctl_finder_user_helper <kernel src directory>");
        exit(-1);
    }

    fd = open(DEVICE_FILENAME, O_RDWR);

    n = scandir("/dev", &namelist, filter, alphasort);
    if (n < 0) {
        perror("scandir");
    } else {
        while (n--) {
            memset(&req, 0, sizeof(struct ioctl_finder_req));
            snprintf(req.dev_path, sizeof(req.dev_path) - 1, "/dev/%s", namelist[n]->d_name);

            free(namelist[n]);

            ioctl(fd, IO_IOCTL_FINDER, &req);

            output_unlocked_symbol = calloc(OUTPUT_SIZE, sizeof(char));
            output_compat_symbol = calloc(OUTPUT_SIZE, sizeof(char));
            unlocked_symbol_src = calloc(OUTPUT_SIZE, sizeof(char));
            compat_symbol_src = calloc(OUTPUT_SIZE, sizeof(char));

            if (get_kernel_symbol(output_unlocked_symbol, req.response.unlocked_ioctl_p)) {
                output_unlocked_symbol = NULL;
            }

            if (get_kernel_symbol(output_compat_symbol, req.response.compat_ioctl_p)) {
                output_compat_symbol = NULL;
            }

            if (req.response.unlocked_ioctl_p) {
                if (output_unlocked_symbol && kernel_src) {
                    if (find_ioctl_symbol_src(unlocked_symbol_src, output_unlocked_symbol, kernel_src)) {
                        unlocked_symbol_src = NULL;
                    }
                }
                if (output_compat_symbol && kernel_src) {
                    if (find_ioctl_symbol_src(compat_symbol_src, output_compat_symbol, kernel_src)) {
                        compat_symbol_src = NULL;
                    }
                }
                printf("dev: %s\n\tmodule: %s\n\tunlocked_ioctl: [%s] %s (0x%lx)\n\tcompact_ioctl: [%s] %s (0x%lx)\n\n", 
                        req.dev_path,
                        req.response.dev_owner_name[0] ? req.response.dev_owner_name : "No owner module",
                        unlocked_symbol_src ? unlocked_symbol_src : "-", 
                        output_unlocked_symbol ? output_unlocked_symbol : "None",
                        req.response.unlocked_ioctl_p,
                        compat_symbol_src ? compat_symbol_src : "-", 
                        output_compat_symbol ? output_compat_symbol : "None",
                        req.response.compat_ioctl_p);
                if (unlocked_symbol_src) {
                    free(unlocked_symbol_src);
                    unlocked_symbol_src = NULL;
                }
                if (compat_symbol_src) {
                    free(compat_symbol_src);
                    compat_symbol_src = NULL;
                }
            }
            if (output_unlocked_symbol) {
                memset(output_unlocked_symbol, 0, OUTPUT_SIZE);
            }
            if (output_compat_symbol) {
                memset(output_compat_symbol, 0, OUTPUT_SIZE);
            }
        }
        if (namelist) {
            free(namelist);
        }
    }

    close(fd);
    return 0;
}