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
if [ "$MQTT_ENABLED" -ne 1 ]
then
    die "MQTT Support is disabled in the configuration."
fi

OPTIONS="-i ${HOSTNAME}.${SCRIPT}.pub -h ${MQTT_HOST} -p ${MQTT_PORT} -u ${MQTT_USER} -P ${MQTT_PASS} ${MOSQUITTOPUBOPTS} ${MOSQUITTOOPTS}"


[ "${MQTT_STATUSINTERVAL}" -lt 30 ] && MQTT_STATUSINTERVAL=30

while true
do
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/leds/blue"   -m "$( blue_led status        )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/leds/yellow" -m "$( yellow_led status      )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/leds/ir"     -m "$( ir_led status          )"

    mosquitto_pub ${OPTIONS} -t "${TOPIC}/ir_cut"      -m "$( ir_cut status          )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/night_mode"  -m "$( night_mode status      )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/flip"        -m "$( flip status            )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/mirror"      -m "$( mirror status          )"

    mosquitto_pub ${OPTIONS} -t "${TOPIC}/brightness"  -m "$( get_brightness         )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/contrast"    -m "$( get_contrast           )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/hue"         -m "$( get_hue                )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/saturation"  -m "$( get_saturation         )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/sharpness"   -m "$( get_sharpness          )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/denoise"     -m "$( get_denoise            )"

    mosquitto_pub ${OPTIONS} -t "${TOPIC}/rtsp"        -m "$( get_daemon_state rtspd )"
    mosquitto_pub ${OPTIONS} -t "${TOPIC}/system"      -m "$( system_status          )"
    sleep ${MQTT_STATUSINTERVAL}
done
