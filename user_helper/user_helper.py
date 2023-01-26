import os

IOCTL_FINDER_DEV_NAME = "/dev/ioctl_finder_dev"
IOCTL_FINDER_IOCTL_IMPL = 244

def main():
    dir = "/dev"

    ioctl_finder = os.open(IOCTL_FINDER_DEV_NAME, os.O_WRONLY)

    for filename in os.listdir(dir):
        f_path = os.path.join(dir, filename)
        if not os.path.isdir(f_path):
            try:
                ret = os.write(ioctl_finder, str.encode(f_path))
                #if ret == IOCTL_FINDER_IOCTL_IMPL:
                #    print("{0} - IOCTL Exist".format(f_path))
            except Exception as e:
                if e.errno == IOCTL_FINDER_IOCTL_IMPL:
                    print("{0} - IOCTL Exist".format(f_path))
    
    os.close(ioctl_finder)

if __name__ == "__main__":
    main()
