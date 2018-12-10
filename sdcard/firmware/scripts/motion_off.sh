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
    echo "MQTT Support is disabled in the configuration."
    exit 0
fi

echo "$( date ) - Motion OFF" | tee -a "$LOGDIR/motion.log"
mqtt_send "${MOTION_TOPIC}" "OFF"

