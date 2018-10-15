[![Build Status](https://travis-ci.org/fliphess/chuangmi-720p-hack.svg?branch=master)](https://travis-ci.org/fliphess/chuangmi-720p-hack)
[![Docker Repository on Quay](https://quay.io/repository/fliphess/chuangmi-720p-hack/status "Docker Repository on Quay")](https://quay.io/repository/fliphess/chuangmi-720p-hack)

# Chuangmi-720P-hack project

This projects aims at providing an alternate firmware for the Xiaomi Chuangmi 720p IP Camera's.
These camera's, based on the Grain Media GM8136S SOC, normally only work using the cloudbased app.
With this firmware I'll try to offer other methods of using this webcam that do not require an internet uplink.

![Alt text](chuangmi.jpg?raw=true "Chuangmi 720P camera")


This project exists thanks to the extensive research of [jymbob](https://github.com/jymbob/chuangmi-720-hack) and [ghoost](https://github.com/ghoost82/mijia-720p-hack) and all the hard work done on existing hacks for other Xiaomi ip camera's done by many people on the webs. Standing on the heads of giants, much respect and thanks go out to those who shared their efforts to create a more suitable firmware for their Xiaomi ip camera.

If I somehow screwed up in licensing a script or tool that I've copied from your project, please file an issue, so I can add the requested changes. (I really suck at licensing, but I'd like to do it right)

## WARNING - DISCLAIMER

**Many files on the Chuangmi 720P are writable. Be very careful when you modify files on it, you might brick it forever.**

Although the hack should be fully reversible by removing the sdcard from the camera's slot, it's very easy to screw things up on this camera and create a very shiney paperweight object that doesn't do camera-ing anymore.

I'll test every change on my own chuangmi cams, but I cannot guarantee that it keeps running smoothly if you change files that are not on the sdcard.

Always use the releases rather than the latest master or development branches if you want to be sure of a more or less tested setup ;)

## Features

This project is a collection of scripts and binaries file to hack your Xiaomi Chuangmi 720P camera.

This camera has the default following features:

* Wifi
* Night vision
* Motion detection: a video file is generated if a motion have been detected in the last 60 seconds.
* Send video data over the network on Chinese servers in the cloud to allow people to view camera data from their smartphone wherever they are.
* Setup thanks to a smartphone application.
* Local video storage on a SD card
* No RTSP server


This hack includes:

* No more cloud feature (nothing goes out of your local network)
* No more need to use a smartphone application
* Telnet server - _Disabled by default._
* Web server with PHP support - _Enabled by default._
* HTTP api for control of webcam.
* RTSP server - _Enabled by default._
* SSH server - _Enabled by default._
* FTP server - _Disabled by default._
* Samba Server - _Disabled by default._
* Syslog to memory card - _Enabled by default and not to be disabled currently._
* Configure Timezone and use ntpclient to set date and time over Internet
* Confgurable cronjobs
* MQTT status updates
* MQTT Control


Planned futures:
* Wiki showing how to integrate with home automation
* Configuration over web server
* Replace Chinese voice files with English
* Reintroduction of motion detection without the need for cloud
* Auto update mechanism for firmware
* Capture images using http requests
* MQTT updates and control in php using the existing mosquitto libs on the camera
* User authentication on RTSP daemon
* Remote syslogging
* Tamper detection
* Audio with RTSP stream


Please read the security considerations at the bottom of the readme.

Also, it might be smart not to buy this camera at all, as it is not very secure.
If you have some at home that are not doing anything, this project could offer some additional features that makes the camera a bit more usable, but do not buy this product to use it with this hack :)

Buy a secure camera that is usable and up to date out of the box if you're smart :)


## Installation on the Chuangmi 720P camera

The memory card must stay in the camera ! If you remove it, the camera will start without using the hack.

### Build the binaries

To build the binaries the GM8136 SDK toolchain must be installed in /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE
You can download them from [my website](https://fliphess.com/toolchain) or use the docker container to setup a build environment.

Clone this repository on a computer:
```
git clone https://github.com/fliphess/chuangmi-720p-hack.git
```

Then change into the cloned directory, build the binaries and install them to the sdcard base directory

```
cd chuangmi-720p-hack
make && make images
```

Or, if you use the docker container, build images by running `./manage.sh --all` on a computer running docker to create a fully working archive containing all the needs.
This is the recommended method as it will install and configure all dependencies for you.

Alternatively you can download the prebuild releases from the [github releases page](https://github.com/fliphess/chuangmi-720p-hack/releases) that are build by travis on every release.

### Prepare the memory card

You can use the self compiled image from the cloned repository or download a precompiled release.
Format a micro SD card in fat32 (vfat) format and copy the content of the **sdcard/** folder at the root of your memory card.

### Configure the Chuangmi camera on the memory card

To configure the wifi network to use, edit the file **config.cfg**.
To configure the services which should run on the camera, open the file **config.cfg** and set the values.

If you configured the wifi already using the mii app, the settings in `config.cfg` are not required, but still recommended.
If your nvram somehow gets corrupted, the settings in `config.cfg` can be used to reset the wifi.


## Start the camera

* If plugged, unplug the Chuangmi camera
* Insert the memory card in the Chuangmi camera
* Plug the Chuangmi camera

The camera will start. The led will indicate the current status:
* yellow: camera startup
* blue blinking: network configuration in progress (connec to wifi, set up the IP address)
* blue: network configuration is OK. Camera is ready to use.

You can test is your camera is up and running this hack with your browser on url **http://your-camera-ip/**.


## Resetting the firmware if the hack is not working

If the hack is not working, use the `tf_recovery.img` to restore to an older u-boot version that is able to read from the sd card at boot time.

To do this, put the `tf_recovery.img` in the root of the sdcard and remove any other files. If you are using a mac, add the sdcard to your spotlight private excludes in your settings to prevent the `.Spotlight` directory to be created, which could block the reset.

Wait for 10 minutes till the blue led goes on and unplug the camera. The `tf_recovery.img` file is renamed to `tf_recovery.img.bak` if the reset completed successfully.


## Use the camera

### Telnet server

If enabled the telnet server is on port 23.

Default login/password:
* login = root
* password = Chuangmi720pCam123 (unless you specified another password in **config.cfg** file)

### SSH server

If enabled the SSH server is on port 22.

Default login/password:
* login = root
* password = Chuangmi720pCam123 (unless you specified another password in **config.cfg** file)

### RTSP Server

If enabled the RTSP server is on port 554.

You can connect to live video stream (currently only supports 720p) on:
```
rtsp://your-camera-ip/live/ch00_0
```

For stability reasons it is recommend to disable cloud services while using RTSP.

### FTP server

If enabled the FTP server is on port 21.

There is no login/password required.


### MQTT Support

If enabled the MQTT support, a status update is pushed to the broker every N seconds.
By pushing commands to $MQTT_TOPIC/set, many values can be changed.

I'm still working on the completion of this feature. Have a look at the `MQTT_*` configuration options in `config.cfg` and file an issue if something is unclear.
This forces me to write more extensive documentation for the new features I've added soon.


### Samba

If enabled the CHUANGMI_RECORD_VIDEO directory can be accessed via CIFS.
The share is readable by everyone.

Default login/password for read/write access:
* login = root
* password = Chuangmi720pCam123 (unless you specified another password in **config.cfg** file)


## Cloud Services

Disabling the cloud services disables the following functions:

* Motion detection
* No video data or configuration with the smartphone application
* No recordings on the SD card or a remote file system

For stability reasons it is recommend to disable cloud services while using RTSP.


## Security considerations

The short version:

DO NOT EVER PUBLICLY EXPOSE THIS WEBCAM TO THE INTERNET !

This is not a very secure webcam. Although it is semi-safe to use for home automation projects, exposing it to the internet would backfire quickly and is considered very [badong](https://www.urbandictionary.com/define.php?term=badong).

It's running an old u-boot version, a kernel from the middle ages and was never designed to be exposed to the internet.
The kernel has no iptables or any other traffic filtering mechanism build in, so securing this webcam from the big angry internet is not easily done.

Use it with common sense: Use secure strong passwords, use a private and secure wifi network, put it behind a firewall, don't show your junk in front of the webcam and make sure don't place it in private spaces.
As the rtsp daemon is not secured with authentication, anyone in your network is able to view the camera feed, which could include the guy in the hoody leeching your wifi.

I use this camera's on a private wifi dedicated to IOT devices. It has no connection to the internet but NTP and is not accessible from my users wifi other than through zoneminder.
This way i feel i've at least tried to prevent others from being able to access my webcams for evil.

Although the camera was originally designed as a baby monitor, I would not recommend to use it that way without some additional security layers, with or without the hack.
Don't look at me when your sauna pictures (or worse) show up on the internet: I warned you ;)

## Uninstall the hack

There are no files altered on the camera so simply remove the SD card to uninstall the hack.

## Need help? Want to help? Ideas? suggestions?

Feel welcome to bring pull requests and feature requests on the table.

As there are many limitations to the camera, join the [gitter channel](https://gitter.im/chuangmi-720p-hack) if you want to start a discussion on how to do things.
I cannot guarantee that I've got (time to build) all the answers and solutions, but let's see if we can tackle some nice features together :)

As I'm not that experienced with writing user interfaces, I would really appreciate it if some more experienced PHP devs and interface pro's would take a look at the web interface code for security and efficiency reasons.
If you're good at making a nice web interface, let me know! I'm quite tasteless when it comes to the web and layout, so I could use some help :)

I love to learn, which is why i forked this project to work on a chuangmi specific firmware, but as a single developer I'm pretty sure, I make ugly mistakes.
Please file issues and PR's if you notice something that could've been handled in a better way. I'm open to all suggestions and I would love to see some engagement of other nerds that bought this camera by accident or on purpose :)

