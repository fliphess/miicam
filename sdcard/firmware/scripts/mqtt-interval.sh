#!/bin/sh

## Set scriptname
SCRIPT="$( basename ${0} )"

## Set mountdir
SD_MOUNTDIR="/tmp/sd"

########################################
## Load functions                     ##
########################################

if [ -r "${SD_MOUNTDIR}/firmware/scripts/functions.sh" ]
then
    . "${SD_MOUNTDIR}/firmware/scripts/functions.sh"
else
    echo "Unable to load basic functions"
    exit 1
fi


########################################
## Bail out if MQTT is disabled in config.cfg
########################################

if [ "${ENABLE_MQTT}" -ne 1 ]
then
    die "MQTT Support is disabled in the configuration."
fi


########################################
## Load MQTT functions                ##
########################################

if [ -r "${SD_MOUNTDIR}/firmware/scripts/mqtt-triggers.sh" ]
then
    . "${SD_MOUNTDIR}/firmware/scripts/mqtt-triggers.sh"
else
    echo "Unable to load MQTT functions"
    exit 1
fi

########################################
## Overrule logfile                   ##
########################################

LOGFILE="${SD_MOUNTDIR}/log/mqtt.log"


########################################
## Override to 30 if a lower value is set
########################################

[ "${MQTT_STATUSINTERVAL}" -lt 30 ] && MQTT_STATUSINTERVAL=30

########################################
## The main loop                      ##
########################################

while sleep ${MQTT_STATUSINTERVAL}
do
    log "MQTT Update: Sending status update to mqtt broker"
    led_blue_status
    led_yellow_status
    led_ir_status
    get_led_status

    ir_cut_status
    night_mode_status
    mirror_mode_status
    flip_mode_status
    get_mode_status

    brightness_status
    contrast_status
    hue_status
    saturation_status
    denoise_status
    sharpness_status
    get_camera_status

    rtsp_status
    get_system_status
done
