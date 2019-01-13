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

## Overrule logfile
LOGFILE="${SD_MOUNTDIR}/log/mqtt.log"

## Logger function
log() {
    echo "$( date ) - ${*}" | tee -a ${LOGFILE}
}


## Bail out if MQTT is disabled in config.cfg
if [ "$ENABLE_MQTT" -ne 1 ]
then
    die "MQTT Support is disabled in the configuration."
fi


## Set status interval to 30 if a lower value is set
[ "${MQTT_STATUSINTERVAL}" -lt 30 ] && MQTT_STATUSINTERVAL=30

while sleep ${MQTT_STATUSINTERVAL}
do
    log "MQTT Update: Sending status update to mqtt broker"

    mqtt_send "${MQTT_TOPIC}/leds"              "$( led_status   )"
    mqtt_send "${MQTT_TOPIC}/leds/blue"         "$( blue_led status   )"
    mqtt_send "${MQTT_TOPIC}/leds/yellow"       "$( yellow_led status )"

    mqtt_send "${MQTT_TOPIC}/leds/ir"           "$( /tmp/sd/firmware/bin/ir_led -s | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/leds/ir/pwm"       "$( /tmp/sd/firmware/bin/ir_led -j )"

    mqtt_send "${MQTT_TOPIC}/ir_cut"            "$( /tmp/sd/firmware/bin/ir_cut -s | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/ir_cut/pins"       "$( /tmp/sd/firmware/bin/ir_cut -j )"

    mqtt_send "${MQTT_TOPIC}/night_mode"        "$( /tmp/sd/firmware/bin/nightmode -s | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/night_mode/ev"     "$( /tmp/sd/firmware/bin/nightmode -j )"

    mqtt_send "${MQTT_TOPIC}/mirror_mode"       "$( /tmp/sd/firmware/bin/mirrormode -s | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/mirror_mode/ev"    "$( /tmp/sd/firmware/bin/mirrormode -j )"

    mqtt_send "${MQTT_TOPIC}/flip_mode"         "$( /tmp/sd/firmware/bin/flipmode -s | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/flip_mode/ev"      "$( /tmp/sd/firmware/bin/flipmode -j )"

    mqtt_send "${MQTT_TOPIC}/flip_mode"         "$( /tmp/sd/firmware/bin/flipmode -s | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/flip_mode/ev"      "$( /tmp/sd/firmware/bin/flipmode -j )"

    mqtt_send "${MQTT_TOPIC}/brightness"        "$( /tmp/sd/firmware/bin/camera_adjust -g -t brightness | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/contrast"          "$( /tmp/sd/firmware/bin/camera_adjust -g -t contrast   | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/hue"               "$( /tmp/sd/firmware/bin/camera_adjust -g -t hue        | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/saturation"        "$( /tmp/sd/firmware/bin/camera_adjust -g -t saturation | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/denoise"           "$( /tmp/sd/firmware/bin/camera_adjust -g -t denoise    | awk '{print $NF}' )"
    mqtt_send "${MQTT_TOPIC}/sharpness"         "$( /tmp/sd/firmware/bin/camera_adjust -g -t sharpness  | awk '{print $NF}' )"

    mqtt_send "${MQTT_TOPIC}/rtsp"              "$( get_daemon_state rtspd )"
    mqtt_send "${MQTT_TOPIC}/system"            "$( system_status )"
    mqtt_send "${MQTT_TOPIC}/camera/modes"      "$( mode_status   )"
    mqtt_awns "${MQTT_TOPIC}/camera/settings"   "$( /tmp/sd/firmware/bin/camera_adjust -j )"
done
