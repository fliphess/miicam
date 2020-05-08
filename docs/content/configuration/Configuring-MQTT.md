# Configuring MQTT

When enabled in `config.cfg`, it is possible to control many features of the hack over MQTT.

The camera sends an update every few seconds, reporting its status.
By pushing commands to `$MQTT_TOPIC/set`, many values can be changed.

## Services

There are 2 scripts that enable MQTT control. Both use the `config.cfg` for configuration.

These scripts are:

- mqtt-control.sh  - Used for managing incoming MQTT messages
- mqtt-interval.sh - Used for sending a status update every N seconds

Both scripts can be started from the web interface or by using its init script:

```
/tmp/sd/firmware/etc/init/S99mqtt-interval
/tmp/sd/firmware/etc/init/S99mqtt-control
```

If `ENABLE_MQTT` is set to `1` in `config.cfg`, both services will be started at boot time.

## Known issues

You should not use MQTT topics that contain whitespaces in the topic string.
The MQTT scripts can't handle a topic with a space or tab in it.

## Configuration options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_MQTT`            | `1` to enable, `0` to disable. | Enable the MQTT Functionality. |
| `MQTT_USER`              | The username string            | The username for connecting to the MQTT server |
| `MQTT_PASS`              | The password string            | The password for connecting to the MQTT server |
| `MQTT_HOST`              | The hostname string            | The hostname or ip address of the MQTT server  |
| `MQTT_PORT`              | The server port int            | The port of the MQTT server |
| `MQTT_TOPIC`             | The base mqtt topic string     | The base MQTT topic to subscribe to |
| `MOSQUITTOOPTS`          | Option string                  | The additional options string for `mosquitto-sub` |
| `MOSQUITTOPUBOPTS`       | Option string                  | The additional options string for `mosquitto-pub` |
| `MQTT_STATUSINTERVAL`    | Wait time in seconds           | The wait time between MQTT status updates |


## Config example

The configuration section for MQTT in `config.cfg`:

```bash
############################################################
## MQTT Settings                                          ##
############################################################

## Enable or disable MQTT service
ENABLE_MQTT=0

## Connection options for broker server
MQTT_USER=mqtt-user
MQTT_PASS=mqtt-password
MQTT_HOST=mqtt-host
MQTT_PORT=1883

## Define the base topic used by the camera
MQTT_TOPIC="home/$HOSTNAME"

## Define additional options for Mosquitto here.
## For example --cafile /tmp/sd/firmware/etc/cacert.pem --tls-version tlsv1
MOSQUITTOOPTS=""

## Add options for mosquitto_pub like -r for retaining messages
MOSQUITTOPUBOPTS="-r "

## Send a mqtt statusupdate every n seconds
MQTT_STATUSINTERVAL=30
```

## Interval

Depending on `MQTT_STATUSINTERVAL` in `config.cfg`,
the mqtt-interval service will send a value to the following channels:

```bash
## Led topics
${MQTT_TOPIC}/leds/blue
${MQTT_TOPIC}/leds/yellow
${MQTT_TOPIC}/leds/ir

## Sensor topics
${MQTT_TOPIC}/ir_cut
${MQTT_TOPIC}/night_mode
${MQTT_TOPIC}/flip
${MQTT_TOPIC}/mirror

## Image topics
${MQTT_TOPIC}/brightness
${MQTT_TOPIC}/contrast
${MQTT_TOPIC}/hue
${MQTT_TOPIC}/saturation
${MQTT_TOPIC}/sharpness
${MQTT_TOPIC}/denoise

## System topics
${MQTT_TOPIC}/rtsp
${MQTT_TOPIC}/system
```

If the `MQTT_STATUSINTERVAL` is set below 30 seconds,
it will be reset to 30, to prevent the script from running too often.


## Control MQTT

By sending a command to a preconfigured MQTT topic, you can control several options of the camera.
At boot time the mqtt-control service is starting which listens on the following topics:

```bash
## Led control
${MQTT_TOPIC}/leds/blue/set on
${MQTT_TOPIC}/leds/blue/set off
${MQTT_TOPIC}/leds/blue
${MQTT_TOPIC}/leds/yellow/set on
${MQTT_TOPIC}/leds/yellow/set off
${MQTT_TOPIC}/leds/yellow
${MQTT_TOPIC}/leds/ir/set on
${MQTT_TOPIC}/leds/ir/set off
${MQTT_TOPIC}/leds/ir

## Camera control
${MQTT_TOPIC}/ir_cut/set on
${MQTT_TOPIC}/ir_cut/set off
${MQTT_TOPIC}/ir_cut
${MQTT_TOPIC}/brightness/set *
${MQTT_TOPIC}/brightness
${MQTT_TOPIC}/contrast/set *
${MQTT_TOPIC}/contrast
${MQTT_TOPIC}/hue/set *
${MQTT_TOPIC}/hue
${MQTT_TOPIC}/saturation/set *
${MQTT_TOPIC}/saturation
${MQTT_TOPIC}/denoise/set *
${MQTT_TOPIC}/denoise
${MQTT_TOPIC}/sharpness/set *
${MQTT_TOPIC}/sharpness

## System settings
${MQTT_TOPIC}/rtsp/set on
${MQTT_TOPIC}/rtsp/set off
${MQTT_TOPIC}/rtsp
${MQTT_TOPIC}/system

## Night mode
${MQTT_TOPIC}/night_mode/set on
${MQTT_TOPIC}/night_mode/set off
${MQTT_TOPIC}/night_mode
${MQTT_TOPIC}/night_mode/auto/set on
${MQTT_TOPIC}/night_mode/auto/set off
${MQTT_TOPIC}/night_mode/auto
```


## Troubleshooting MQTT

As there are many moving parts when controlling the camera with MQTT, debugging issues can be daunting.
To get you going, I've created a list with things to rule out when solving issues with MQTT.

Assuming you have editted your configuration file and configured the correct settings... (IE you're not a complete idiot)

#### Check if your credentials are correct

If you need authentication (which I recommend), make sure you use the correct credentials and test if you
are allowed to send messages to the topic you want to use.

#### Check in the log of the MQTT server for incoming connections

If the camera can connect to the MQTT server to send it's status updates, you should see the loglines of those incoming connections
in the mosquitto logfile (usually in `/var/log/mosquitto/mosquitto.log`).

As each message is send with a new connection, it can be noisy in the logs of connections and disconnections of the camera.

#### Check if you can access the MQTT server from the camera

Make sure the credentials you are using are correctly set in the `config.cfg`, check if the port is correct and
if you have access from the camera to the server using telnet:
```
source /tmp/sd/config.cfg
telnet $MQTT_HOST $MQTT_PORT
```

#### Send a message from the command line

To test where the error appears, use the same function the MQTT utils use to send messages by sourcing the `functions.sh` file:

```
source /tmp/sd/firmware/scripts/functions.sh

mqtt_send "$TOPIC" "some test message"
```

#### Connect to a topic and publish to it

Open 2 terminals to the camera, and use one to listen to a topic:

```
source /tmp/sd/firmware/scripts/functions.sh

mqtt_subscribe "$CAMERA_HOSTNAME/test" "testing_miicam"
```

Now open a second terminal and send a message to the same topic as read ealier:

```
source /tmp/sd/firmware/scripts/functions.sh

mqtt_send "$CAMERA_HOSTNAME/test"  "some test message"
```


Now check in the first terminal if the message arrived by the listening client.

