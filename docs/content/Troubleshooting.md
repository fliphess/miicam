# Troubleshooting

Did the installation not work out? To help you fix the most common issues,
I've created this article with some steps that can help you debug.


!!! note
    Before you start debugging or reading any logs,
    set `ENABLE_LOGGING` to `1` and `PURGE_LOGFILES_AT_BOOT` to `0` in `config.cfg`.
    This enables logging so hopefully all related errors are written to a log file in `/tmp/sd/log` and keeps the log files over reboots.

## General recommendations

## Creating an issue

If you create an issue, most of the "it's not working", "the camera is not booting" or "nothing happens" related cases, I will ask you to pack your log files together and paste them in the issue.
Without these log files we cannot help you debug the issues you ran into.

### Creating an issue

If you create an issue, most of the "it's not working", "the camera is not booting" or "nothing happens" related cases, I will ask you to pack your log files together and paste them in the issue.
Without these log files we cannot help you debug the issues you ran into.

To pack the log files together, I've created some instructions. You can find them in [this article about packing your log files](/How-to-tar-or-zip-the-logs-when-filing-an-issue.md)

### Changing the root Password

Do not use characters in the `ROOT_PASSWORD` or other passwords that are not allowed in shell environments without escaping.
Be careful with using backticks, exclamation marks (`!`), dollar sign ($), semicolon (`;`),  number sign (`#`) and double and single quotes in passwords and settings.

Allowed chars in the web interface config editor are:

```
a-z A-Z 0-9 (){}.@|+:[]><\/^%&_- .=
```

### Editing your config.cfg

Use an editor that does not change the character encoding of the `config.cfg` to make your edits. If you see errors in your log files similar to:

```
Jan  1 00:00:21 wifi: Line 6: failed to parse psk '"^M"'.

/tmp/sd/config.cfg: line 6: ^M: not found
```

This happens if you have edited your `config.cfg` on windows computer with a non-text-based editor.

Use `notepad++` or similar to work around this issue.

You can try to fix this using the `fromdos` utility:

```
/tmp/sd/firmware/bin/fromdos /tmp/sd/config.cfg
```

### WIFI connection issues

Sometimes configuring the WIFI through the `config.cfg` results in a non-responsive or offline camera.
If you run into this behaviour, try configuring the WIFI through the MiiHome app, as the camera is using this state waiting for the app to send the credentials.

If the WIFI connection is not fully setup within a certain amount of time, the WIFI is switched to a state used by the MiiHome app to send the WIFI credentials.

This issue results in the following entries in the logging in `/tmp/sd/log`:
```
Jan 1 00:00:20 wifi: Enabling wifi STA mode
Jan 1 00:00:27 wifi: udhcpc (v1.19.4) started
Jan 1 00:00:27 wifi: Sending discover...
Jan 1 00:00:30 wifi: Sending discover...
Jan 1 00:00:30 wifi: Sending discover...
Jan 1 00:00:30 wifi: Sending discover...
And so on...
```

To work around this issue:

- Remove the SD card from the camera
- Connect the camera to power
- Wait till the STA mode is active and configure the camera's WIFI connection using the MiiHome app.
- Power off the device
- Reinsert the SD card
- Power on again

After the WIFI is set up using the MiiHome, the credentials are stored in nvram and are not needed in the config.cfg anymore.

After you completed the configuration of the camera in the app, the credentials are saved and you don't need to set the password in the `config.cfg` anymore and the camera should join the WIFI as expected.


## The RTSP stream is colored blue

Try switching the IR Cut on or off. Most of the time this is not a bug but only the infra red filter being enabled causing the image to be a bit more blue then red.


## The hack appears not to be working

If the hack is not working, try the procedures below to get the camera working:

### Flash the u-boot firmware

If you haven't flashed to an older u-boot using the `tf_recovery.img`, [do this first](/Flashing-the-U-boot-firmware-to-an-older-version-when-the-hack-is-not-working).

After you have inserted the sd card with only the `tf_recovery.img` on it, start the camera and wait until the blue led starts blinking.


### Install the firmware on the SD card

Because the repo itself does not contain the binaries that are required to run this hack, simply cloning the repository and copying the contents of the `sdcard/` directory to
 SD card, will not suffice. The binaries are build for every release using [travis-ci](https://travis-ci.org/MiiCam/MiiCam/) and are not in the repo.

It is therefor recommended for new users to use the tarbal or zip files that are created with every release.
These releases are the only versions that are tested thoroughly.

You can download the latest on the [releases page](https://github.com/MiiCam/MiiCam/releases).

After you've downloaded the tgz file, unpack the tarbal using the following command:
```
tar xvzf chuangmi-720p-hack.tgz
```

...And copy the contents of the `sdcard/` directory to the root of your SD card.


Make sure you have installed all the files in the `sdcard/` to the root of the SD card:

```
firmware/
ft_config.ini
manufacture.bin
config.cfg
ft/
tf_recovery.img.bak
```

## The RTSP link when added to VLC player is asking for a login. What should I add?

The authentication on the RTSP service can be configured by setting the corresponding auth variables in `config.cfg`.

The RTSP credentials are by default set to:

| Var         | Value    |
| ---         | ---      |
| `RTSP_USER` | `stream` |
| `RTSP_PASS` | `bJ2xnahtCgninraelmI` |

**Before using the rtsp make sure you change these credentials!**

If you completely unset the variables or change one of both into an empty value (`""`),
the authentication is disabled.


## What is the default password to login using SSH

The `root` user password is by default set to: `Chuangmi720pCam123`.

You should change the password in `config.cfg`.


## Unstable camera / Sudden reboots

If the camera is constantly rebooting and or slow, keep in mind that this is a very small camera with a very small brain.

If you enable all functionality including (especially) the Xiaomi cloud suite,
the web server, the MQTT scripts and the RTSP server,
there is a big chance the camera gets very hot very often and suddenly reboots.

As it is not a computer,
I'm afraid you have to make a smaller selection of functionality you desire and disable the rest.

It is best to leave the cloud suite disabled at all times and only use the RTSP service instead.
You can use all the functionality of the hack at the same time
as long as you do not enable the cloud suite at the same time.


## The RTSP stream is glitched and stuttering

See: "Unstable camera".

(Disable the Xiaomi cloud functionality)


## The cloud suite is disabled but the RTSP stream is still glitching

If you disabled the cloud suite in the configuration (which it is by default),
and you are still running into glitchy stream, it is possible there are too many viewers for the camera.
Use a RTSP proxy to lower the connections to the camera.

Another possibility is that the camera is still running the cloud suite due to an error in the boot routine.
If this is the case, look at the log files on your camera for errors and create an issue, so we can debug it together.


## The camera is bricked

In most of the cases flashing the camera using the `tf_recovery.img` is enough to restore a bricked camera.


## I deleted files on the camera and now it's not booting anymore.

If you deleted files outside the SD card and the camera is not working functionally anymore,
I'm afraid there is not much you can do.

You can try using the `tf_recovery.img` file to restore the u-boot, but I have no idea if this will repair the damage.


## The SD card is not detected.

I've heard about some issues when using newer SDHC cards. Try using an older SD card below 32GB.

## The camera smells like burning plastic

Disconnect the power immediately (Stop reading and do it NOW!),
and keep a close eye on the plastic to ensure no flames or smoke are coming from the camera 🔥

If needed, drown the camera in a glass of water or a toilet to ensure any starting fire is gone and check the warranty.

Ask for a refund or buy a replacement as you are dealing with serious hardware failure. Do not reconnect the power!


## Pictures of my mom that are taken with this camera are all over the internet

You didn't read [the security considerations](/Security-Considerations) didn't you?
