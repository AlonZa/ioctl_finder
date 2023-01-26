obj-m := ioctl_finder.o
ioctl_finder-objs += core.o \
					utils.o ioctl_finder_dev_handler.o

INCLUDE = include/utils.h include/ioctl_finder_dev_handler.h
SRC_FILES = utils.c ioctl_finder_dev_handler.c core.c

all: $(SRC_FILES) $(INCLUDE)
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	rm -r -f *.mod.c .*.cmd *.symvers *.o *.mod *.order
	mkdir --parents build
	mv ioctl_finder.ko build/

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
