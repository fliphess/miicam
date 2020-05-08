[![Build Status](https://travis-ci.org/miicam/MiiCamUpdater.svg?branch=master)](https://travis-ci.org/miicam/MiiCamUpdater)

# MiiCamUpdater

This is a very basic updater for the miicam firmware hack.

It uses rsync and a few scripts to push the files of the 
newer build to the camera and generates a new config file.

This tool is not recommended for beginning users and is meant for miicam developers 
have the need to quickly switch versions of the camera build. 

This tool has only been tested on MacOSX and Linux 
and requires `ssh`, `rsync` and `python-requests`


## Utils

This package provides 2 utilities: `miicam-downloader` and `miicam-updater`.


### Downloader

`miicam-downloader` is a utility to quickly download the latest release or nightly build. 
By default the files are unpacked in /tmp.

To download the latest release:

```bash
miicam-downloader --release
```


To download the latest nightly build:

```bash
miicam-downloader --nightly
```


### Updater


`miicam-updater` is a utility to push the downloaded files to the camera using rsync. 

After you've downloaded the latest release or nightly build using the `miicam-downloader` utility, 
You can upload the files to the camera using the updater utility:

```bash

miicam-updater --source /tmp/miicam --host camera3 --port 22 --user root --verbose
```
