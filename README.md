[![Build Status](https://travis-ci.org/miicam/MiiCam.svg?branch=master)](https://travis-ci.org/miicam/MiiCam)
[![Docker Repository on Quay](https://quay.io/repository/miicam/miicam/status "Docker Repository on Quay")](https://quay.io/repository/miicam/miicam)
[![GitHub commit activity](https://img.shields.io/github/commit-activity/m/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam/pulse)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/MiiCam/MiiCam.svg)](https://github.com/miicam/MiiCam)
[![GitHub All Releases](https://img.shields.io/github/downloads/MiiCam/MiiCam/total.svg)](https://github.com/miicam/MiiCam/releases)
[![Anaconda-Server Badge](https://anaconda.org/miicam/miicam-nightly/badges/version.svg)](https://anaconda.org/miicam/miicam-nightly/files)


# MiiCam hack project

```
MiiCam is retiring!

Maintaining a webcam project like this one in a secure manner is hard.
As this camera is getting older, it needs more and more attention to determine whether it is still safe to use this firmware
and I don't feel motivated enough to keep doing this while I'm almost not using the camera anymore.

It's not a very powerfull camera and at this point this limits my ideas and options for my home automation setup,
which is why I bought newer, more powerfull camera's with more options
that didn't have the limitations I experience with the chuangmi camera.

In a few months, I will stop maintaining this project.
If other developers using this firmware are interested in taking over: I'm open to answering all questions
and helping with project based tasks and advice on how to step in.
Please contact me on discourse.

If nobody else wants to take over the maintenance of this firmware,
on October 1st 2020 I will archive the project.

To protect the lesser skilled from downloading very insecure and outdated firmwares years after I archived the project,
I will remove all current CI pipelines, development containers, prebuilds and releases and the nightly builds as well.
```

This projects provides an alternate firmware for the Xiaomi Chuangmi 720p IP Camera's.
These camera's, based on the Grain Media GM8136S SOC, normally only work using the cloudbased app.
With this firmware an alternate method of using this webcam is created that does not require an internet uplink.

**Please read the [security considerations](https://miicam.github.io/Security-Considerations).**

![Camera image](https://github.com/MiiCam/MiiCamWeb/raw/master/public/static/images/chuangmi.jpg "Chuangmi 720P camera")


## Documentation

Tave a look in the documentation for [all the available options and features](https://miicam.github.io).


## Installation on the Chuangmi 720P camera

To install the hack on your SD card, use the compiled sources and a SD card with a max size of 32GB.
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

