#!/bin/sh

## Set scriptname
SCRIPT="$( basename ${0} )"


## Set mountdir
SD_MOUNTDIR="/tmp/sd"


## Load functions
if [ -r "${SD_MOUNTDIR}/firmware/scripts/functions.sh" ]; then
    . "${SD_MOUNTDIR}/firmware/scripts/functions.sh"
else
    echo "Unable to load basic functions"
    exit 1
fi


## Bail out if MQTT is disabled in config.cfg
if [ "$ENABLE_MQTT" -ne 1 ]
then
    die "MQTT Support is disabled in the configuration."
fi


## Set status interval to 30 if a lower value is set
[ "${MQTT_STATUSINTERVAL}" -lt 30 ] && MQTT_STATUSINTERVAL=30

while sleep ${MQTT_STATUSINTERVAL}
do
    mqtt_send "${MQTT_TOPIC}/leds/blue"   "$( blue_led status        )"
    mqtt_send "${MQTT_TOPIC}/leds/yellow" "$( yellow_led status      )"
    mqtt_send "${MQTT_TOPIC}/leds/ir"     "$( ir_led status          )"

    mqtt_send "${MQTT_TOPIC}/ir_cut"      "$( ir_cut status          )"
    mqtt_send "${MQTT_TOPIC}/night_mode"  "$( night_mode status      )"
    mqtt_send "${MQTT_TOPIC}/flip"        "$( flip status            )"
    mqtt_send "${MQTT_TOPIC}/mirror"      "$( mirror status          )"

    mqtt_send "${MQTT_TOPIC}/brightness"  "$( get_brightness         )"
    mqtt_send "${MQTT_TOPIC}/contrast"    "$( get_contrast           )"
    mqtt_send "${MQTT_TOPIC}/hue"         "$( get_hue                )"
    mqtt_send "${MQTT_TOPIC}/saturation"  "$( get_saturation         )"
    mqtt_send "${MQTT_TOPIC}/sharpness"   "$( get_sharpness          )"
    mqtt_send "${MQTT_TOPIC}/denoise"     "$( get_denoise            )"

    mqtt_send "${MQTT_TOPIC}/rtsp"        "$( get_daemon_state rtspd )"
    mqtt_send "${MQTT_TOPIC}/system"      "$( system_status          )"
done

