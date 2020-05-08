# Configuring RTSP

The RTSP daemon can be used to create a local video stream instead of using the cloud functionality.

**The RTSP daemon is required for using the snapshot and recording functions in the web interface.**

## Enabling the RTSP daemon

**WARNING: The RTSP daemon has authentication but without encryption! 
Read the [Security Considerations](/Security-Considerations) for more information**

By default, the rtspd is enabled. You can disable it by setting `ENABLE_RTSP` to `0` and stop the rtspd service
on the commandline by executing the init script, or through the services page in the web interface.

If enabled the RTSP server is listening on port `554`.

You can connect to live video stream (currently only supports 720p) on:
```
rtsp://your-camera-ip/live/ch00_0
```

**For stability reasons disable cloud services while using RTSP.**


## Manually starting, stopping and requesting the status of the RTSP service

Using the init script, you can easily start and stop the rtsp daemon:

```bash
## Start
/etc/init/S99rtsp start

## Stop
/etc/init/S99rtsp stop

## Restart
/etc/init/S99rtsp restart

## Retrieve the status
/etc/init/S99rtsp status
```

## Authentication

Since `v0.959` the rtsp stream uses authentication to keep unwanted viewers from watching you in your underpants.

Set the `RTSP_USER` and `RTSP_PASS` to something you can remember.

To completely disable the authentication, set the variables to `RTSP_PASS=` and `RTSP_USER=` (to an empty string).

## Changing settings of the RTSP stream

You can change several settings of the RTSP daemon within the limits of the camera.

If you want to change the frame-rate (FPS) settings, 
test for the best possible results as not all settings changes work out well. 
You can change the frame-rate to above 15fps, but effectively it will not go much higher than 20 frames per second.

The camera is very picky about some settings. 
If the camera stream seems glitchy or the camera is periodically rebooting, 
change the RTSP settings in `config.cfg` back to its default.


## Recommended settings

During tests the quality and overall performance of the camera was at best using a variable bitrate, 
as the camera can skip some frames when it's under stress.

Using the `GM_EVBR` mode or `GM_VBR` does not seem to make much difference but try for yourself 
if you experience issues with the settings you are using.

The recommended settings for best performance and quality are:

| Encoding | Bitrate Mode   | (Max) Bitrate | FPS  | Width | Height |
| ----     | ----           | ----          | ---- | ----  | ----   |
| `H264`   | `GM_EVBR` (`4`)| 8192          | 15   | 1280  | 720    |
| `MPEG4`  | `GM_EVBR` (`4`)| 2048          | 10   | 1280  | 720    |
| `MJPEG`  | `GM_CBR`  (`1`)| 4096          | 15   | 1280  | 720    |

**When there is low light and the camera has lots of data to process, for example when the images are rapidly changing, 
using a bitrate that is too high for the camera can result in a crashing or glitchy rtsp stream. 
If this happens, try lowering the max bitrate (IE: from 8092 to 4096 or lower).**

**Since v0.961 the rtspd service allows setting a higher frame-rate than 15 fps. 
This increases power usage and may cause instability of the camera.**

## Available Bitrate Modes

The available bitrate modes are:

| Bitrate Mode | Bitrate Mode Variable | Description
| ----         | ----                  | ----
| 1            | `GM_CBR` 	           | [Constant Bitrate](https://en.wikipedia.org/wiki/Constant_bitrate).
| 2            | `GM_VBR`              | [Variable Bitrate](https://en.wikipedia.org/wiki/Variable_bitrate).
| 3            | `GM_ECBR`             | Enhanced Constant Bitrate.
| 4            | `GM_EVBR`             | Enhanced Variable Bitrate.


## Encoder type option of the rtspd binary.

The RTSP service supports multiple encoder types.
These settings are accepted as command line arguments of the `rtspd` binary.

As `H264` gives the best results in performance and image quality, this is the recommended and default encoding used.
You can use `Mjpeg` and `mpeg4` as well, by adding the command flag to the RTSP_EXTRA_ARGS variable in `config.cfg`.


### Changing the encoder type

To switch to another encoding, you can add the command flag to the RTSP_EXTRA_ARGS.
This adds the flag to the commandline arguments that are used to start the daemon.


The default startup options are:

```bash
DAEMON_OPTS=" -f$RTSP_FRAMERATE -w$RTSP_WIDTH -h$RTSP_HEIGHT -b$RTSP_BITRATE -m$RTSP_BITRATE_MODE $RTSP_EXTRA_ARGS"
```

If you want to use MJPEG, the additional command line option you need to add to `RTSP_EXTRA_ARGS` is the `-j` flag, 
if you want to use `MPEG4` the required flag is `-4`.
To use `H264` encoding, no additional edits are needed as this is the default option.

For example, to configure MJPEG encoding, change the `RTSP_EXTRA_ARGS` variable in `config.cfg` to:

```bash
RTSP_EXTRA_ARGS=" -j"
```

Now save your changes and restart the RTSP service as described ealier in this article.


### Testing your changes

There are 2 test we can do to verify whether our changes are effective: 
Check the command line arguments of the RTSP service and second: use ffmpeg to verify which encoding is used.

First we'll check the command line arguments:

```bash
ps -ef | grep rtsp
```

This should result the process of the RTSP service and the commandline arguments used:
```
  519 root     274:52 /tmp/sd/firmware/bin/rtspd -f15 -w1280 -h720 -b8091 -m1 -j
```

Now verify if the correct encoding is used by retrieving the stream with ffprobe:
```bash
# ffprobe rtsp://localhost:554/live/ch00_0
ffprobe version 4.0.2 Copyright (c) 2007-2018 the FFmpeg developers
  built with gcc 4.4.0 (Buildroot 2012.02) 20100318 (experimental)
  configuration: --pkg-config-flags=--static --extra-cflags=-I/build/prefix/include --extra-cxxflags=-I/build/prefix --extra-ldflags=-L/build/prefix/lib --arch=arm --target-os=linux --cross-prefix=arm-unknown-linux-uclibcgnueabi- --prefix=/build/prefix --disable-shared --disable-ffplay --disable-doc --disable-w32threads --enable-avcodec --enable-avformat --enable-avfilter --enable-swresample --enable-swscale --enable-ffmpeg --enable-filters --enable-gpl --enable-iconv --enable-libx264 --enable-nonfree --enable-pthreads --enable-runtime-cpudetect --enable-small --enable-static --enable-version3 --enable-zlib
  libavutil      56. 14.100 / 56. 14.100
  libavcodec     58. 18.100 / 58. 18.100
  libavformat    58. 12.100 / 58. 12.100
  libavdevice    58.  3.100 / 58.  3.100
  libavfilter     7. 16.100 /  7. 16.100
  libswscale      5.  1.100 /  5.  1.100
  libswresample   3.  1.100 /  3.  1.100
  libpostproc    55.  1.100 / 55.  1.100
Input #0, rtsp, from 'rtsp://localhost:554/live/ch00_0':
  Metadata:
    title           : RTSP Server Live
    comment         : ICL Streaming Media
  Duration: N/A, start: 0.000000, bitrate: N/A
    Stream #0:0: Video: h264, yuv420p(progressive), 1280x720, 15.08 tbr, 90k tbn, 180k tbc
```

In the metadata you can see the `h264` encoding is being used:

```
Stream #0:0: Video: h264, yuv420p(progressive), 1280x720, 15.08 tbr, 90k tbn, 180k tbc
```

## Motion detection

The rtspd has motion detection functionality. To enable it, 
have a look at the [docs about motion detection](/configuration/Setting-up-motion-detection)


## Recording and snapshots

You can create snapshot images and record video using the RTSP software. 
This can be both on command and when motion is detected.

To create a snapshot from the command line use the `take_snapshot` utility. 
To start a recording of 30 seconds, use `take_video`.


## Configuration options

### RTSP Settings

The options for the RTSP service are:

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_RTSP`            | `1` to enable, `0` to disable. | Enable or disable the rtspd service |
| `RTSP_USER`              | The username string to connect | Set to enable password authentication |
| `RTSP_PASS`              | The password string to connect | Set to enable password authentication |
| `RTSP_WIDTH`             | An integer below `1280`        | Set the image width of the rtsp stream |
| `RTSP_HEIGHT`            | An integer below `720`         | Set the image height of the rtsp stream |
| `RTSP_FRAMERATE`         | An integer below `15`          | Set the max fps of the rtsp stream |
| `RTSP_BITRATE`           | An integer below `8192`        | Set the max bitrate of the rtsp stream |
| `RTSP_BITRATE_MODE`      | An integer between `0` and `4` | Set the bitrate mode of the rtsp stream |
| `RTSP_EXTRA_ARGS`        | A command line string          | Set extra commandline arguments to the rtspd |


## Configuration example

```bash
## Start local streaming server at system boot (0/1)
ENABLE_RTSP=1

## The username to connect to the rtsp stream
RTSP_USER="stream"

## The password to connect to the rtsp stream
RTSP_PASS="bJ2xnahtCgninraelmI"

## The image width of the RTSP stream
RTSP_WIDTH=1280

## The image height of the RTSP stream
## Downscaling is possible
RTSP_HEIGHT=720

## Set the max framerate of the RTSP daemon.
## A framerate of 15 fps is the max supported by the camera
RTSP_FRAMERATE=15

## The max bitrate
RTSP_BITRATE=8192
RTSP_BITRATE_MODE=4

## Extra command line arguments for the RTSP daemon
RTSP_EXTRA_ARGS=""
```

