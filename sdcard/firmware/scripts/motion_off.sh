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

## Turn IR Led off
if [ "$MOTION_LIGHT_ALERT" -eq 1 ]
then
    ir_led off
fi

## Message MQTT
if [ "$ENABLE_MQTT" -eq 1 ]
then
    mqtt_send "${MOTION_TOPIC}" "$MOTION_MQTT_OFF"
fi

echo "$( date ) - Motion: $MOTION_MQTT_OFF" | tee -a "$LOGDIR/motion.log"

