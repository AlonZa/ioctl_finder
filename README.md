# ioctl_finder

Kernel module that finds if a device implements `unlocked_ioctl`.

## Build

1. `cd src`
2. `make`

All the compiled file (kernel module and user helper) will be under the `build/` directory.

## Execution

Using `root`:
1. Load the module using - `insmod build/ioctl_finder.ko`
Usage:
  2.1. Run the user helper to get information on all the devices under `/dev` - `./build/ioctl_finder_user_helper`
  2.2 To get a source code location of the `ioctl()`, you may add the linux sources path to the execution command as such: `./build/ioctl_finder_user_helper /path/to/kernel/sources`

## Additional Info

The module registering a char device in the path `/dev/ioctl_finder_dev`. This device handles a `_IOWR` IOCTL.
The IOCTL recieves a `struct ioctl_finder_req` (see `src/include/utils.h`), and respond by filling the `response` field in the struct.
You may use this information to create a new user helper with more features then the current, very basic, user helper.
