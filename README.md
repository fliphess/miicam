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
* Samba Server - _Disabled by default._
* Syslog to memory card - _Enabled by default and not to be disabled currently._
* Configure Timezone and use ntpclient to set date and time over Internet
* Confgurable cronjobs
* MQTT status updates
* MQTT Control


Planned futures:

* Wiki showing how to integrate with home automation
* Web based Image and Video viewer
* Build a newer `tf_recovery.img` and u-boot using the toolchain
* Capture video on motion and on command
* Configuration over web server
* Replace Chinese voice files with English and more important: allow playback of those voices
* Reintroduction of motion detection without the need for cloud by extending the current rtspd code
* Auto update mechanism for firmware
* MQTT updates and control
* User authentication on RTSP daemon
* SSL for webserver and rtspd
* Remote syslogging
* Audio with RTSP stream


## Installation on the Chuangmi 720P camera

The memory card must stay in the camera!
If you remove it, the camera will start without using the hack.


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

If the hack is not working, result [the wiki for all known issues and workarounds](https://github.com/fliphess/chuangmi-720p-hack/wiki/Troubleshooting)


## Use the camera

Using the web interface and over MQTT many settings and services can be enabled and disabled.

Have a look at the api docs in the web interface for a list of all available http calls you can use to control the camera.

The MQTT page on the wiki will soon reflect all available options for controlling and monitoring the camera using MQTT.

## Services

Have a look at the [wiki services page](https://github.com/fliphess/chuangmi-720p-hack/wiki/Configuring-Services) for all available additional services this firmware offers.


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
