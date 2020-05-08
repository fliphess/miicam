# Setting up motion detection

Using the RTSP service, motion detection can be enabled to send MQTT messages or create snapshots 
when movement is detected in the image stream.


## Motion detection mechanism

The mechanism that does image analysis and detects motion changes is build in the rtsp service.

When motion is detected, a script is triggered:
```bash
/tmp/sd/firmware/scripts/motion_on.sh
```

When no motion is detected anymore, another script is triggered:
```bash
/tmp/sd/firmware/scripts/motion_off.sh
```

The scripts by default does not much, but if you enable MQTT in the configuration options,
a MQTT message is sent to a topic defined in the config file.

## Extending On-motion actions

You can add some actions yourself by editing the scripts in `/tmp/sd/firmware/scripts`.

By extending the `/tmp/sd/firmware/scripts/motion_on.sh` and `/tmp/sd/firmware/scripts/motion_off.sh` scripts,
any action can be performed when motion is detected.

For example if you want to make a HTTP request when motion is detected, 
you can add a http request using `curl` or `wget`
. This makes it very powerful and extendable to add additional actions when motion is detected.


Make sure you requests don't take too long to avoid lots of hanging curl requests 
because the request take too long. You can use `flock` to prevent http requests from being executed in parallel:
```bash
flock -n /tmp/.motion.lock -c 'curl -qs https://some.service/api/motion/detected'
```

## Snapshot

The RTSP service can take snapshots of the image stream that are saved on the sd card.
On the command line you can execute the `take_snapshot` utility to create a snapshot.

If you set `MOTION_TAKE_SNAPSHOT` and `MOTION_DETECTION` to `1`, a snapshot is stored on the sd card when motion is detected.

## Recording

You can configure the RTSP service to record a video of 20 seconds when motion is detected. 
While motion is detected, the camera keeps recording with a max of 30 seconds. 
After 30 seconds a new video file is created.

You can download the last created video file from the web interface, but there is not a real video browser at the moment as the video's are not browser viewable yet.

I've documented the routine of [converting the video using ffmpeg](/development/Convert-recorded-video)

## Configuration Options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `MOTION_DETECTION`       | `1` to enable, `0` to disable  | Enable motion detection |
| `MOTION_TAKE_SNAPSHOT`   | `1` to enable, `0` to disable  | Take an image snapshot when motion is detected |
| `MOTION_RECORD`          | `1` to enable, `0` to disable  | Record 10 seconds when motion is detected |
| `MOTION_MQTT_ON`         | A string, int or bool          | The string to send when motion is detected |
| `MOTION_MQTT_OFF`        | A string, int or bool          | The string to send when no motion detected anymore |

## Configuration example

```bash
## Enable motion detection
MOTION_DETECTION=1

## Enable snapshot on motion detection
MOTION_TAKE_SNAPSHOT=0

## Enable video recording on motion detection
MOTION_RECORD=0

## What to publish over MQTT when motion is detected
## Set ENABLE_MQTT=1 to enable
MOTION_MQTT_ON="ON"
MOTION_MQTT_OFF="OFF"

## Which topic to publish to
MOTION_TOPIC="$MQTT_TOPIC/motion"
```
