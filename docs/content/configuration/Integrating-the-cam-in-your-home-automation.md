# Integrating the cam in your home automation

As I only use one home automation suite myself (home automation), 
I can't test all the instructions to integrate the camera in your home setup of choice. 
Please let me know if the instructions are not up to date anymore.

If you use a setup that is not listed here, 
please document your configuration steps so I can add it to this article.


## Using Home assistant

To view the camera in your home assistant setup, 
use the [ffmpeg component](https://www.home-assistant.io/components/camera.ffmpeg/):

```
- platform: ffmpeg
  name: Chuangmi
  input: -rtsp_transport tcp -i rtsp://camera:554/live/ch00_0
```

How to control your camera in home assistant is a bit up to you as you should decide for yourself 
which functionality you want to make controllable through home automation.

For example, you can choose to create buttons to enable and disable the leds 
or the rtspd service using [mqtt switches](https://www.home-assistant.io/components/switch.mqtt/):

```
switch:
  # Change blue led
  - platform: mqtt
    name: "Camera1 Blue Led"
    icon: "mdi:led-on"
    state_topic: "home/camera1/leds/blue"
    command_topic: "home/camera1/leds/blue/set"
    payload_on: "on"
    payload_off: "off"

  # Change rtsp service on/off
  - platform: mqtt
    name: "Camera1 MJPEG RTSP Server"
    icon: "mdi:cctv"
    state_topic: "home/camera1/rtsp"
    command_topic: "home/camera1/rtsp/set"
    payload_on: "on"
    payload_off: "off"
```

(Untested but mostly copied from my own hass)

... Or create [sensors](https://www.home-assistant.io/components/sensor.mqtt/) 
or [binary sensors](https://www.home-assistant.io/components/binary_sensor.mqtt/) for topics/outputs 
that are not controllable but only send status updates... 
(binary_sensor if the output is 1/0 (on/off) and sensors for anything else):

```
sensor:
  - platform: mqtt
    name: Camera1 Light Sensor
    state_topic: "home/camera1/brightness"
    unit_of_measurement: "%"
    icon: "mdi:brightness"
```

The output of system_status is:

```
{"uptime":" 20:52:54 up  4:06,  1 users,  load average: 21.25, 21.24, 21.23","ssid":"gnoffel","bitrate":"72.2 Mb/s","signal_level":"-57","link_quality":"87%","noise_level":"0"}
```

Using templates you can split each part to it's own sensor:

```
sensor:
  - platform: mqtt
    name: Camera1 Noise Level
    state_topic: "home/camera1/system"
    unit_of_measurement: "%"
    value_template: '{{ value_json.noise_level.replace("%", "") | int }}'
```

Or track the system load:

```
  - platform: mqtt
    name: Camera1 Load 1min
    state_topic: "home/camera1/system"
    unit_of_measurement: ''
    value_template: '{{ value_json.uptime | regex_findall_index("[\:|,]\ ([0-9\.]+)", -3) | float }}'

  - platform: mqtt
    name: Camera1 Load 5min
    state_topic: "home/camera1/system"
    unit_of_measurement: ''
    value_template: '{{ value_json.uptime | regex_findall_index("[\:|,]\ ([0-9\.]+)", -2) | float }}'

  - platform: mqtt
    name: Camera1 Load 15min
    state_topic: "home/camera1/system"
    unit_of_measurement: ''
    value_template: '{{ value_json.uptime | regex_findall_index("[\:|,]\ ([0-9\.]+)", -1) | float }}'
```


Have a look [in the source code to](https://github.com/miicam/MiiCam/blob/master/sdcard/firmware/scripts/mqtt-interval.sh) 
[see which commands are available](https://github.com/miicam/MiiCam/blob/master/sdcard/firmware/scripts/mqtt-control.sh) 
for control.

I would not recommend to map each functionality and part of the output the camera spits out 
over mqtt to sensors/switches etc but first determine what you want to do and only create configuration to do that. 
Home assistant changes a lot and having to control all your configuration at every update is a bit of a pita 
so keep your config as small as possible and only create what you need :)


## Using Home Bridge

To add this camera to homebridge then to homekit, you need HOMEBRIDGE installed somewhere (on a raspberry PI for instance)

When homebridge is configured and running, 
install the [homebridge-camera-ffmpeg](https://github.com/KhaosT/homebridge-camera-ffmpeg) plugin.

To configure the plugin, use the following json config in the platform category:

```json
{
  "platform":"Camera-ffmpeg",
  "cameras":[
     {
        "name":"Camera 1",
        "videoConfig":{
           "source":"-rtsp_transport tcp -re -i rtsp://USER:PASS@CAMERA_HOSTNAME:554/live/ch00_0",
           "stillImageSource":"-rtsp_transport tcp -re -i rtsp://USER:PASS@CAMERA_HOSTNAME:554/live/ch00_0 -vframes 1",
           "maxStreams":2,
           "maxWidth":720,
           "maxHeight":480,
           "maxFPS":15,
           "vflip":false,
           "hflip":false,
           "vcodec":"h264_omx",
           "debug": true
        }
     }
  ]
}
```

To use the snapshot, u need to manually edit the `ffmpeg.js` file of the plugin 
to modify the timeout settings used in the ffmpeg command line arguments.

Change the timeout for snapshot creation from 1s to 5s (Or alternatively, 
until this change is implemented upstream you can [use this fork](https://github.com/epalzeolithe/homebridge-camera-ffmpeg))

