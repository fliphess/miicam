[![Build Status](https://travis-ci.org/fliphess/chuangmi-720p-hack.svg?branch=master)](https://travis-ci.org/fliphess/chuangmi-720p-hack)
[![Docker Repository on Quay](https://quay.io/repository/fliphess/chuangmi-720p-hack/status "Docker Repository on Quay")](https://quay.io/repository/fliphess/chuangmi-720p-hack)

# Chuangmi-720P-hack project

## WARNING - DISCLAIMER

**Many files on the Chuangmi 720P are writable. Be very careful when you modify files on it, you might brick it forever.**

## Purpose

This project is a collection of scripts and binaries file to hack your Xiaomi Chuangmi 720P camera.

![Alt text](chuangmi.jpg?raw=true "Chuangmi 720P camera")

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
* RTSP server - _Enabled by default._
* SSH server - _Enabled by default._
* FTP server - _Disabled by default._
* Samba Server - _Disabled by default._
* Syslog to memory card - _Disabled by default._
* Configure Timezone and use ntpclient to set date and time over Internet

Planed futures:
* Configuration over web server
* Replace Chinese voice files with English

## Installation on the Chuangmi 720P camera

The memory card must stay in the camera ! If you remove it, the camera will start without using the hack.

### Build the binaries

To build the binaries the GM8136 SDK toolchain must be installed in /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE

Clone this repository on a computer:

    git clone https://github.com/fliphess/chuangmi-720p-hack.git

Then change into the cloned directory, build the binaries and install them to the sdcard base directory

    cd firmware
    make
    make install

Or use the docker container by running `./build.sh` on a computer running docker.

### Prepare the memory card

You can use the self compiled image from the cloned repository or download a precompiled release.

Then, format a micro SD card in fat32 (vfat) format and copy the content of the **firmware/sdcard/** folder at the root of your memory card.

The memory card will so contain:

* ft: folder which contains the hack entry point
* ft_config.ini and manufacture.bin: files needed to enable the hack
* firmware: the folder which contains the hack scripts and binaries
* config.cfg: configuration file to configure the hack
* log: this folder will contains some log files from the hack and if enabled the system logs
* CHUANGMI_RECORD_VIDEO: this folder will only be created when some video records will be added on the memory card by the camera

### Configure the Chuangmi camera on the memory card

To configure the wifi network to use, edit the file **config.cfg**.

To configure the services which should run on the camera, open the file **config.cfg** and set the values.

## Start the camera

* If plugged, unplug the Chuangmi camera
* Insert the memory card in the Chuangmi camera
* Plug the Chuangmi camera

The camera will start. The led will indicate the current status:
* yellow: camera startup
* blue blinking: network configuration in progress (connec to wifi, set up the IP address)
* blue: network configuration is OK. Camera is ready to use.

You can test is your camera is up and running this hack with your browser on url **http://your-camera-ip/**.

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

rtsp://your-camera-ip/live/ch00_0

For stability reasons it is recommend to disable cloud services while using RTSP.

### FTP server

If enabled the FTP server is on port 21.

There is no login/password required.

### MQTT Support

If enabled the MQTT support, a status update is pushed to the broker every N seconds.
By pushing commands to $TOPIC/set, many values can be changed.


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

## I want more !

Some scripts are provided in the **sd/firmware/scripts** folder. Please read the **README.md** file in this folder for more informations.

Uninstall the hack
==================

There are no files altered on the camera so simply remove the SD card to uninstall the hack.


How it works ?
==============

Hack content
------------

```
ft/                            Folder that contains the start script for the hack
ft_config.ini                  Neccessary configuration file for the hack boot
manufacture.bin                Archive that contains the script test_drv that will enable the hack
firmware/            Chuangmi 720O hack folder
 bin/                          Contains server and system binaries for the hack
 etc/                          Configuration files for the services provided by the hack
 scripts/                      Some scripts
 www/                          root of the erb server
config.cfg                     Chuangmi 720O hack configuration file
````

