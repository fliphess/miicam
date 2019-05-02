[![Build Status](https://travis-ci.org/miicam/MiiCam.svg?branch=master)](https://travis-ci.org/miicam/MiiCam)
[![Docker Repository on Quay](https://quay.io/repository/miicam/miicam/status "Docker Repository on Quay")](https://quay.io/repository/miicam/miicam)
[![GitHub commit activity](https://img.shields.io/github/commit-activity/m/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam/pulse)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam)
[![GitHub All Releases](https://img.shields.io/github/downloads/MiiCam/MiiCam/total.svg)](https://github.com/miicam/MiiCam/releases)
[![Anaconda-Server Badge](https://anaconda.org/miicam/miicam-nightly/badges/version.svg)](https://anaconda.org/miicam/miicam-nightly/files)


# MiiCam hack project

This projects provides an alternate firmware for the Xiaomi Chuangmi 720p IP Camera's.
These camera's, based on the Grain Media GM8136S SOC, normally only work using the cloudbased app.
With this firmware an alternate method of using this webcam is created that does not require an internet uplink.

**Please read the [security considerations](https://miicam.github.io/Security-Considerations).**

![Camera image](https://github.com/MiiCam/MiiCamWeb/raw/master/public/static/images/chuangmi.jpg "Chuangmi 720P camera")


## Documentation

Tave a look in the documentation for [all the available options and features](https://miicam.github.io).


## Installation on the Chuangmi 720P camera

To use install the hack on your SD card, use the compiled sources, and a SD card with a max size of 32GB.
Copying the files in the repository to your sd card without building the binaries wil **Not** work.

In the docs you can find [instructions on how to install the hack](https://miicam.github.io/getting-started).


### Download the firmware hack

For new users it is recommended to use the precompiled binaries that are created with every release.
You can download the release builds from the [github releases page](https://github.com/MiiCam/MiiCam/releases).


### Build the binaries

If you prefer to build the binaries yourself, [have a look at the instructions using docker](https://miicam.github.io/development/How-to-build-the-binaries-for-the-webcam-hack).


## Security considerations

The short version: **DO NOT EVER PUBLICLY EXPOSE THIS WEBCAM TO THE INTERNETZ!**

The longer version [can be read on its own page](https://miicam.github.io/Security-Considerations).


## Issues and bugs

If you discover bugs, please [create an issue](https://github.com/miicam/MiiCam/issues), so we can look for a fix :)
If you can fix it yourself, feel very welcome to create pull requests.

If there is a bug in the documentation, [create an issue in the docs repo](https://github.com/miicam/MiiCamDocs/issues).
And for the web interface, [create an issue in the repo for the web interface](https://github.com/miicam/MiiCamWeb/).


## Getting Help

I've spend many hours documenting anything I ran into.

Please read the [troubleshooting guide](https://miicam.github.io/Troubleshooting) for solving the most common issues.

If something is not documented, [please let me know](https://github.com/miicam/MiiCam/issues) so I can extend the documentation.


Please always do some proper research yourself first and only create issues if you are very sure it's a bug in the camera and not either human error or another issue that is related to your home setup instead of the camera.

If you need help setting up, ask the friendly people on the discord channel for some advice. If you answer their questions properly and give them the required information, they might be willing to point you in the right direction.

I'm not that good (read: not socially equipped) with helping people setting up their home automation, I'd rather work on new functionality for the camera.

If your issue is not related to the camera or does not contain enough information to start searching for the issue,
I might respond itchy or close and lock issues straight after creation to prevent the issue tracker from filling up.
If you think it's an actual bug and I was to quick with closing your issue, please don't be offended but create a new bugreport with more details on why you think it's a bug or send me a message on discord.

Keep in mind that the issue tracker is not a helpdesk or community forum, but a bug tracker for an open source project.

