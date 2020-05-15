[![Build Status](https://travis-ci.org/miicam/MiiCam.svg?branch=master)](https://travis-ci.org/miicam/MiiCam)
[![GitHub commit activity](https://img.shields.io/github/commit-activity/m/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam/pulse)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam)
[![GitHub All Releases](https://img.shields.io/github/downloads/MiiCam/MiiCam/total.svg)](https://github.com/miicam/MiiCam/releases)


# MiiCam hack project


## MiiCam is retiring!

Maintaining a webcam project like this one in a secure manner is a lot of work!
As this camera is getting older, it needs more and more attention to determine keep it up to date and (semi) "secure"
Because I'm not using the camera that much myself anymore, I will archive the project on October 1st 2020.

In a few months, I will stop maintaining this project.

I've already removed the prebuild releases as the older releases were getting really old...
I recommend the poweruser to build their own firmware using the instructions,
and less tech savy users to start looking for a better and more modern camera.


## What is this project

This projects provides an alternate firmware for the Xiaomi Chuangmi 720p IP Camera's.
These camera's, based on the Grain Media GM8136S SOC, normally only work using the cloudbased app.
With this firmware an alternate method of using this webcam is created that does not require an internet uplink.

**Please read the [security considerations](https://miicam.github.io/Security-Considerations).**

![Camera image](https://github.com/MiiCam/MiiCam/raw/master/web/public/static/images/chuangmi.jpg "Chuangmi 720P camera")


## Documentation

Tave a look in the documentation for [all the available options and features](https://miicam.github.io).


## Installation on the Chuangmi 720P camera

To install the hack on your SD card, some tech ninja linux hacking skills are required!

Build the required firmware from the git repository by compiling the binaries is docker.

Use a SD card with a max size of 32GB formatted with `FAT32`. (Larger cards and SDXC or SDUC cards are not always working)

For a working firmware, you need the compiled binaries,
simply copying the files in the repository to your sd card without building the firmware in docker wil **Not** work.


### Build the binaries

To build the binaries yourself, [have a look at the instructions for building a firmware using docker](https://miicam.github.io/development/How-to-build-the-binaries-for-the-webcam-hack).


### Upgrading the binaries

To upgrade the firmware, pull the repo, clean and rebuild:

```
git pull && ./manage --all
```

## Security considerations

The short version: **DO NOT EVER PUBLICLY EXPOSE THIS WEBCAM TO THE INTERNETZ!**
The longer version [can be read on its own page](https://miicam.github.io/Security-Considerations).

In fact, forget the theme park and **DO NOT USE THIS WEBCAM FIRMWARE AT ALL BECAUSE IT IS BEING SUNSET SOON!**


## Troubleshooting

Please read the [troubleshooting guide](https://miicam.github.io/Troubleshooting) for solving the most common issues.

