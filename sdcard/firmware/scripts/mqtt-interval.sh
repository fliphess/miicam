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

OPTIONS="-i ${HOSTNAME}.${SCRIPT}.pub -h ${MQTT_HOST} -p ${MQTT_PORT} -u ${MQTT_USER} -P ${MQTT_PASS} ${MOSQUITTOPUBOPTS} ${MOSQUITTOOPTS}"


[ "${MQTT_STATUSINTERVAL}" -lt 30 ] && MQTT_STATUSINTERVAL=30

while sleep ${MQTT_STATUSINTERVAL}
do
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/leds/blue"   -m "$( blue_led status        )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/leds/yellow" -m "$( yellow_led status      )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/leds/ir"     -m "$( ir_led status          )"

    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/ir_cut"      -m "$( ir_cut status          )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/night_mode"  -m "$( night_mode status      )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/flip"        -m "$( flip status            )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/mirror"      -m "$( mirror status          )"

    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/brightness"  -m "$( get_brightness         )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/contrast"    -m "$( get_contrast           )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/hue"         -m "$( get_hue                )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/saturation"  -m "$( get_saturation         )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/sharpness"   -m "$( get_sharpness          )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/denoise"     -m "$( get_denoise            )"

    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/rtsp"        -m "$( get_daemon_state rtspd )"
    mosquitto_pub ${OPTIONS} -t "${MQTT_TOPIC}/system"      -m "$( system_status          )"
done
