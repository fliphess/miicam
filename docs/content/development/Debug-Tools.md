# Debug Tools

To reverse engineer the inner workings of the camera firmware, several debug tools are added.
As the busybox used in the firmware is offering lots of utilities, many of the required shell utilities are present in the firmware.


## Available utilities

Some useful tools are:

- strace
- lsof
- tcpdump
- ffmpeg
- ffprobe
- jq
- fsck



If you miss a specific tool, create a PR or an issue to [get help with implementing new software on the camera](https://miicam.github.io/development/Adding-software-to-the-build/).


## Editors

For editors you have two choices for now: vi and nano. If you prefer a more graphical editor, have a look at `sshfs` to mount the `/tmp/sd` directory of the camera on your local desktop.


## Toolchain and hardware specifics

For firmware specifics and the custom kernel modules used to control the internal hardware of the camera, have a look at [the pdf's in the toolchain documentation](https://github.com/miicam/MiiCamDocs/tree/master/documents).
