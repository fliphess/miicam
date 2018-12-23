[![Build Status](https://travis-ci.org/fliphess/chuangmi-720p-hack.svg?branch=master)](https://travis-ci.org/fliphess/chuangmi-720p-hack)
[![Docker Repository on Quay](https://quay.io/repository/fliphess/chuangmi-720p-hack/status "Docker Repository on Quay")](https://quay.io/repository/fliphess/chuangmi-720p-hack)

# Chuangmi-720P-hack project

This projects provides an alternate firmware for the Xiaomi Chuangmi 720p IP Camera's.
These camera's, based on the Grain Media GM8136S SOC, normally only work using the cloudbased app.
With this firmware I'll try to offer other methods of using this webcam that do not require an internet uplink.

**Please read the [security considerations in the wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki/Security-Considerations).**

![Alt text](https://github.com/fliphess/chuangmi-720p-hack/raw/master/sdcard/firmware/www/public/static/images/chuangmi.jpg "Chuangmi 720P camera")


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
* Syslog to SD card and remote host - _Enabled by default_
* Configure Timezone and use ntpclient to set date and time over Internet
* Configurable cronjobs
* MQTT status updates
* MQTT Control
* Configuration using a basic web interface
* Reintroduction of motion detection without the need for cloud
* User authentication on RTSP daemon
* SSL for webserver (not enabled by default yet)


Planned futures:

* Wiki showing how to integrate with home automation
* Web based Image and Video viewer
* Build a newer `tf_recovery.img` and u-boot using the toolchain
* Capture video on motion and on command
* Replace Chinese voice files with English and more important: allow playback of those voices
* Auto update mechanism for firmware
* MQTT updates and control
* SSL for rtspd
* Audio with RTSP stream


## Installation on the Chuangmi 720P camera

The SD card must stay in the camera!
If you remove it, the camera will start without using the hack.

You can use the pre-compiled firmware that is build with each release or build the firmware yourself using docker or a linux machine.


### Prepare the SD card

Format a micro SD card in fat32 (vfat) format and copy the content of the **sdcard/** folder in the root of your SD card.


### Configure the Chuangmi camera on the SD card

To configure a wifi network to connect to, edit the file **config.cfg**.

If you configured the wifi prior to this hack using the miihome app, the settings are stored in nvram, and not required to set in `config.cfg`.


## Start the camera

* If plugged, unplug the Chuangmi camera
* Insert the SD card in the Chuangmi camera
* Plug the Chuangmi camera

If all is well, the camera will start.

The led will indicate the current status:

* yellow: camera startup
* blue blinking: network configuration in progress (connec to wifi, set up the IP address)
* blue: network configuration is OK. Camera is ready to use.

You can test is your camera is up and running this hack with your browser on url **http://your-camera-ip/**.


## Resetting the firmware if the hack is not working

If the hack is not working, there is a big chance your firmware has been upgraded using the miihome app.
In newer firmware upgrades the boot from sdcard mechanism is blocked.

Have a look in the wiki on [how to downgrade your firmware](https://github.com/fliphess/chuangmi-720p-hack/wiki/Flashing-the-U-boot-firmware-to-an-older-version-when-the-hack-is-not-working) using the `tf_recovery.img` recovery image.


## Using the camera

Using the web interface and over MQTT many settings and services can be enabled and disabled.

Have a look at the api docs in the web interface for a list of all available http calls you can use to control the camera.

For controlling the camera over MQTT, have a look at the [MQTT wiki page](https://github.com/fliphess/chuangmi-720p-hack/wiki/Configuring-MQTT)


## Security considerations

The short version: **DO NOT EVER PUBLICLY EXPOSE THIS WEBCAM TO THE INTERNETZ!**

The longer version can be read in the [wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki):


## Uninstall the hack

There are no files altered on the camera so simply remove the SD card to uninstall the hack.

## Build the binaries

Have a look at the [instructions in the wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki/How-to-build-the-binaries-for-the-webcam-hack) to find instructions on how to build the binaries yourself.

Alternatively you can download the prebuild releases from the [github releases page](https://github.com/fliphess/chuangmi-720p-hack/releases) that are build by travis on every release.


## Need help? Want to help? Ideas? suggestions?

Feel welcome to bring pull requests and feature requests on the table.

If you're good at making a nice web interface, let me know! I'm quite tasteless when it comes to the web and layout, so I could use some help :)

Please file issues and PR's if you notice something that could've been handled in a better way. I'm open to all suggestions and I would love to see some engagement of other nerds that bought this camera by accident or on purpose :)


## More Info

Have a look at the [wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki) to find some additional documentation.
If you struggle setting up your camera using this hack, please file an issue so we can create some additional docs.


