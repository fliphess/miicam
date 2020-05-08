# Configuring the shell environment

There are 2 shells on board that you can use for configuration and exploring the camera: `bash` and `ash` (busybox sh)

## Using bash

To use bash, you can use the `admin` user to login. This user has the `bash` shell configured to be run at login.
If you logged in as `root`, you can use the bash shell by calling it's executable.

(It's full path is `/tmp/sd/firmware/bin/bash` instead of of the common used `/bin/bash`).

The bash shell has no buildins enabled as this didn't work well with the cross-compilation. You are using the busybox shell tools.


## Using Ash (busybox sh)

The default shell environment is a very limited [Almquist shell](https://en.wikipedia.org/wiki/Almquist_shell) or `ash` shell,
provided by [busybox](https://busybox.net) on the readonly filesystem.

For more information have a look at: https://linux.die.net/man/1/ash or look for the `dash` shell,
as both are very similar.

You can create aliases and functions and many features that most shell's offer.


## Creating aliases and adding settings to the shell environment

You can add aliases and settings to the shell environment by editing the `.profile` and the `.busybox_aliases` file.

As the `/root` partition is created at boot-time by mounting a `tmpfs` and copying all files from the sd card,
all changes in `/root/.profile` and `/root/.busybox_aliases` will be gone after a reboot.

This can be useful for testing purposes, but to make settings permanent,
you should add your settings in the source root files in `/tmp/sd/firmware/root`


## Using bash as the default login shell

It is not recommended to use the bash shell as the default shell for `root`. If something goes wrong during boot and the SD card is not available, it becomes very hard to debug the issue as the shell configured in `/etc/passwd` is not available.

The bash shell is on the SD card so the root user uses the default `/bin/sh` shell to guarantee a working shell when the sd card is not able to mount.

The admin user has exactly the same `uid` and `gid` as the root user, but uses the bash shell as login. Use this user if you want to use bash, or change the shell after logging in.

