# ioctl_finder

Kernel module that finds if a device implements `unlocked_ioctl`.

## Build

1. `cd src`
2. `make`

All the compiled file (kernel module and user helper) will be under the `build/` directory.

## Execution

Using `root`:
1. Load the module using - `insmod build/ioctl_finder.ko`
2. Run the user helper to get information on all the devices under `/dev` - `./build/ioctl_finder_user_helper`