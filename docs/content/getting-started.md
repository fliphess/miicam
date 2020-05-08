# Setting up the camera

## Prerequisites

Before getting started with this hack, know that some basic linux knowledge and command line skills are required to use this hack.


## Downloading the files

!!! warning "To use install the hack, always use the **compiled** version."
    Copying just the files in the repository to your sd card without building the binaries wil **Not** work.

As the large static binaries do not work well with git, the repository itself does not contain the compiled binaries.
The binaries will be build by travis and uploaded to github on every release.

For new users it is recommended to use the precompiled binaries that are created with every release.
You can download the release builds from the [github releases page](https://github.com/MiiCam/MiiCam/releases).

### Build the binaries

If you prefer to build the binaries yourself, have a look at the [building documentation](/development/How-to-build-the-binaries-for-the-webcam-hack) to find instructions on how to do it yourself.

## Flash the U-boot version

If the firmware is too recent, the hack cannot always be activated.
To work around this issue, you can downgrade the firmware to a working version [using the flash instructions](/Flashing-the-U-boot-firmware-to-an-older-version-when-the-hack-is-not-working)


### Prepare the SD card

Format a micro SD card in fat32 (vfat) format and copy the content of the **sdcard/** folder in the root of your SD card.


### Configure the Chuangmi camera on the SD card

When the camera is started for the first time, it is waiting for the MiiHome app to send the credentials.
Setup the camera with the MiiHome app, and wait till the camera is connected to the wifi.

To configure another wifi network to connect to, edit the file **config.cfg**.


## Start the camera

* If plugged, unplug the camera
* Insert the SD card in the camera
* Power on the camera

If all is well, the camera will start.

The led will indicate the current status:

* yellow: camera startup
* blue blinking: network configuration in progress (connec to wifi, set up the IP address)
* blue: network configuration is OK. Camera is ready to use.

You can test is your camera is up and running this hack with your browser on url **http://your-camera-ip/**.


## Using the camera

Using the web interface and over MQTT many settings and services can be enabled and disabled.

Have a look at the api docs in the web interface for a list of all available http calls you can use to control the camera.

For controlling the camera over MQTT, have a look at the [MQTT documentation](/configuration/Configuring-MQTT)


## Uninstall the hack

There are no files altered on the camera so simply remove the SD card to uninstall the hack.


