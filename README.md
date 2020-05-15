[![Build Status](https://travis-ci.org/miicam/MiiCam.svg?branch=master)](https://travis-ci.org/miicam/MiiCam)
[![GitHub commit activity](https://img.shields.io/github/commit-activity/m/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam/pulse)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam)
[![GitHub All Releases](https://img.shields.io/github/downloads/MiiCam/MiiCam/total.svg)](https://github.com/miicam/MiiCam/releases)


# MiiCam hack project


## MiiCam is retiring!

Maintaining a webcam project like this one in a secure manner is a lot of work!
As this camera is getting older, it needs more and more attention to determine keep it up to date and (semi) "secure"
Because I'm not using the camera that much myself, I will archive the project on October 1st 2020.

In a few months, I will stop maintaining this project.

I've already removed the prebuild releases as the older were getting really old...
I recommend the poweruser to build their own firmware using the instructions,
and less tech savy users to start looking for a better and more modern camera.


## What is this project

This projects provides an alternate firmware for the Xiaomi Chuangmi 720p IP Camera's.
These camera's, based on the Grain Media GM8136S SOC, normally only work using the cloudbased app.
With this firmware an alternate method of using this webcam is created that does not require an internet uplink.

**Please read the [security considerations](https://miicam.github.io/Security-Considerations).**

![Camera image](https://github.com/MiiCam/MiiCamWeb/raw/master/public/static/images/chuangmi.jpg "Chuangmi 720P camera")


## Documentation

Tave a look in the documentation for [all the available options and features](https://miicam.github.io).


## Installation on the Chuangmi 720P camera

To install the hack on your SD card, some tech ninja linux hacking skills are required!
Build the required firmware from the git repository and use a SD card with a max size of 32GB.
Copying the files in the repository to your sd card without building the binaries wil **Not** work.


### Build the binaries

If you prefer to build the binaries yourself, [have a look at the instructions using docker](https://miicam.github.io/development/How-to-build-the-binaries-for-the-webcam-hack).


## Security considerations

The short version: **DO NOT EVER PUBLICLY EXPOSE THIS WEBCAM TO THE INTERNETZ!**

The longer version [can be read on its own page](https://miicam.github.io/Security-Considerations).


## Issues and bugs

If you discover bugs, please [create a Pull Request fixxing the bug](https://github.com/miicam/MiiCam/pulls), so we can merge your fix :)


## Getting Help

I've spend many hours documenting anything I ran into.

Please read the [troubleshooting guide](https://miicam.github.io/Troubleshooting) for solving the most common issues.

You will find out I'm not a very helpful guy for people using the camera: This is because I am not using the camera anymore and will archive the project soon.


