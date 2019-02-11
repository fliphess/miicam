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

## Message MQTT
if [ "$ENABLE_MQTT" -eq 1 ]
then
    mqtt_send "${MOTION_TOPIC}" "$MQTT_ON"
fi


## Set blue led to blink on motion
if [ "$MOTION_LED" -eq 1 ]
then
    blue_led blink
fi
