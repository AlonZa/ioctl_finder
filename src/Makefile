obj-m := ioctl_finder.o
ioctl_finder-objs += core.o \
					utils.o ioctl_finder_dev_handler.o

INCLUDE = include/utils.h include/ioctl_finder_dev_handler.h
SRC_FILES = utils.c ioctl_finder_dev_handler.c core.c

all: $(SRC_FILES) $(INCLUDE)
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	rm -r -f *.mod.c .*.cmd *.symvers *.o *.mod *.order
	mkdir --parents ../build
	mv ioctl_finder.ko ../build/
	cd ../user_helper
	pwd
	gcc ../user_helper/user_helper.c -o ioctl_finder_user_helper
	mv ioctl_finder_user_helper ../build

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
