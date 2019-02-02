[![Build Status](https://travis-ci.org/miicam/MiiCam.svg?branch=master)](https://travis-ci.org/miicam/MiiCam)

[![Docker Repository on Quay](https://quay.io/repository/MiiCam/MiiCam/status "Docker Repository on Quay")](https://quay.io/repository/MiiCam/MiiCam)

# Chuangmi-720P-hack project

This projects provides an alternate firmware for the Xiaomi Chuangmi 720p IP Camera's.
These camera's, based on the Grain Media GM8136S SOC, normally only work using the cloudbased app.
With this firmware an alternate method of using this webcam is created that does not require an internet uplink.

**Please read the [security considerations in the wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki/Security-Considerations).**

![Alt text](https://github.com/fliphess/chuangmi-720p-hack/raw/master/sdcard/firmware/www/public/static/images/chuangmi.jpg "Chuangmi 720P camera")


## Help debugging

Please debug any issues appearing while using the hack and file a pull request to get things fixed in this repo.
As I'm losing too much time on solving issues that are in the wiki, I've decided to close all issues and start working with a feature tracking tool.

## Features

This project is a collection of scripts and binaries file to hack your Xiaomi Chuangmi 720P camera.
Have a look in [the wiki for all the available options](https://github.com/fliphess/chuangmi-720p-hack/wiki).

## Screenshots

Screenshots of the web interface [can be found in the wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki/Screenshots)


## Installation on the Chuangmi 720P camera

To use install the hack on your SD card, use the compiled sources.
Copying the files in the repository to your sd card without building the binaries wil **Not** work.

###  Get the binaries

For new users it is recommended to use the precompiled binaries that are created with every release.
You can download the release builds from the [github releases page](https://github.com/fliphess/chuangmi-720p-hack/releases).

### Build the binaries

If you prefer to build the binaries yourself, have a look at the [instructions in the wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki/How-to-build-the-binaries-for-the-webcam-hack) to find instructions on how to do it yourself.

### Prepare the camera

When the camera is started for the first time, it is waiting for the MiiHome app to send the credentials.
Setup the camera with the MiiHome app, and wait till the camera is connected to the wifi.

If the firmware is too recent, the hack cannot always be activated.
To work around this issue, you can downgrade the firmware to a working version [using the instructions on the wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki/Flashing-the-U-boot-firmware-to-an-older-version-when-the-hack-is-not-working)


### Prepare the SD card

Format a micro SD card in fat32 (vfat) format and copy the content of the **sdcard/** folder in the root of your SD card.


### Configure the Chuangmi camera on the SD card

To configure a wifi network to connect to, edit the file **config.cfg**.

If you configured the wifi prior to this hack using the MiiHome app, the settings are stored in nvram, and not required to set in `config.cfg`.


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


## Using the camera

Using the web interface and over MQTT many settings and services can be enabled and disabled.

Have a look at the api docs in the web interface for a list of all available http calls you can use to control the camera.

For controlling the camera over MQTT, have a look at the [MQTT wiki page](https://github.com/fliphess/chuangmi-720p-hack/wiki/Configuring-MQTT)


## Security considerations

The short version: **DO NOT EVER PUBLICLY EXPOSE THIS WEBCAM TO THE INTERNETZ!**

The longer version can be read in the [wiki](https://github.com/fliphess/chuangmi-720p-hack/wiki/Security-Considerations)


## Uninstall the hack

There are no files altered on the camera so simply remove the SD card to uninstall the hack.


## Following this repository

As I'm an chaotic idiot that talks to himself a lot, I recommend not to watch this repository, as you will receive lots of very useless notifications.
Choose the "Releases Only" flag to make yourself more zen :)


## Need help? Want to help? Ideas? suggestions?

I've spend many hours documenting everything I ran into in the wiki. Please read the [troubleshooting guide](https://github.com/fliphess/chuangmi-720p-hack/wiki/Troubleshooting) for solving the most common issues.
If something is not documented, please let me know so I can extend the wiki.

Feel welcome to bring pull requests and feature requests on the table.

If you're good at making a nice web interface, let me know! I'm quite tasteless when it comes to the web and layout, so I could use some help :)

