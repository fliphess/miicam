# Getting Help

If you create an issue because "the hack is not working" or "the camera is not booting", I will most of the time ask you to upload the log files of the camera to have a look what is up.

As it's much easier and faster for me to debug a non-responsive camera using the logs, it's recommended to always create a `logs.tar.gz` file containing the log files first, before you continue creating the issue.

In this wiki I'll explain how to upload your log files in a secure and safe way.


!!! warning "Security considerations"
    If things go very bad in the startup routine, it could happen that your WIFI password or root password leaks to the log files.
    Before uploading your log files it is recommended to check if your logs contain secrets that should not be published.


## Requirements

If an issue appears during startup, set `PURGE_LOGFILES_AT_BOOT=0` in your config to ensure the files will not be truncated at startup.

Then power off the camera a few (3 or 4) times and leave the power on for at least 3 minutes after each boot before packing the logs together.


## Create a tar container with all the logs on the camera itself.

If you have telnet or SSH access to the camera, you can tar the files together on the camera itself.

To do this, log in, and issue the following command:
```
cd /tmp/sd
tar cvzf logs.tgz log
```

You can retrieve the `logs.tgz` file using `scp`, `rsync` or `sftp` or by removing the SD card from the camera and inserting it in your computer.

## Create the zip or tar container on your own computer

If no telnet or SSH is running, in most cases some log entries are written to logs anyway, so have a look if any files exist in the `log/` directory on your SD card.

Pack the `log/` using a zip utility or tar the file using your favorite tools and upload in issue :) 👍

Please use only zip or tar and gzip.

## Creating the issue

As debugging is hard without any information, be [very specific](https://www.chiark.greenend.org.uk/~sgtatham/bugs.html) about what is happening and what errors or behaviour you see.

I test all releases thouroghly on multiple camera's so a camera that is not booting or remains nonresponsive camera is in most (like 99%) of the cases the result of a configuration error that you caused yourself.
I've mentioned some in the [Troubleshooting guide](/Troubleshooting), so if you haven't read it, do this first.

Upload the log files if the issue is related to boot issues, otherwise a log file is not needed.


## Feature requests

Although the camera is hard to create features for and as learning to work with the toolkit takes lots of time, creating new features is not always as easy as it sounds.

I have had plans to create ssl encryption for the rtspd and to add audio to the stream,
but as the libraries are undocumented and mostly precompiled, adding functionalities like this is very hard, not to say almost impossible.
I created a [list with planned features](/Planned-Features) that I think I can build om the future.


If you are interested in helping, feel free to create pull requests or ask for more information. I love to see some engagement on this project :)

Feel welcome to adopt some development of any the (listed or unlisted) functionalities if you feel like contributing to this funny little eggformed camera.
