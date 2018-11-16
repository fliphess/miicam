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

if [ "$ENABLE_MQTT" -ne 1 ]
then
    die "MQTT Support is disabled in the configuration."
fi

killall mosquitto_sub 2>/dev/null

OPTIONS="-h $MQTT_HOST -p $MQTT_PORT -u $MQTT_USER -P $MQTT_PASS ${MOSQUITTOOPTS}"

SUB_OPTIONS="-i ${HOSTNAME}.${SCRIPT}.sub ${OPTIONS} -v"
PUB_OPTIONS="-i ${HOSTNAME}.${SCRIPT}.pub ${OPTIONS} ${MOSQUITTOPUBOPTS}"


get_value() {
  INPUT="$1"
  TOPIC="$( echo $INPUT | cut -d " " -f 1 )"
  echo $INPUT | sed -e "s#$TOPIC##g"
}


/usr/bin/mosquitto_sub ${SUB_OPTIONS} -t "${MQTT_TOPIC}/#" | while read -r LINE
do
    case $LINE in
        "${MQTT_TOPIC}/leds/blue/set on")
            blue_led on
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/blue"       -m "$( blue_led status   )"
        ;;
        "${MQTT_TOPIC}/leds/blue/set off")
            blue_led off
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/blue"       -m "$( blue_led status   )"
        ;;
        "${MQTT_TOPIC}/leds/blue")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/blue"       -m "$( blue_led status   )"
        ;;
        "${MQTT_TOPIC}/leds/yellow/set on")
            yellow_led on
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/yellow"     -m "$( yellow_led status )"
        ;;
        "${MQTT_TOPIC}/leds/yellow/set off")
            yellow_led off
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/yellow"     -m "$( yellow_led status )"
        ;;
        "${MQTT_TOPIC}/leds/yellow")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/yellow"     -m "$( yellow_led status )"
        ;;
        "${MQTT_TOPIC}/leds/ir/set on")
            ir_led on
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/ir"         -m "$( ir_led status     )"
        ;;
        "${MQTT_TOPIC}/leds/ir/set off")
            ir_led off
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/ir"         -m "$( ir_led status     )"
        ;;
        "${MQTT_TOPIC}/leds/ir")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/leds/ir"         -m "$( ir_led status     )"
        ;;
        "${MQTT_TOPIC}/ir_cut/set on")
            ir_cut on
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/ir_cut"          -m "$( ir_cut status     )"
        ;;
        "${MQTT_TOPIC}/ir_cut/set off")
            ir_cut off
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/ir_cut"          -m "$( ir_cut status     )"
        ;;
        "${MQTT_TOPIC}/ir_cut")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/ir_cut"          -m "$( ir_cut status     )"
        ;;
        "${MQTT_TOPIC}/brightness/set *")
            set_brightness "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/brightness"      -m "$( get_brightness    )"
        ;;
        "${MQTT_TOPIC}/brightness")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/brightness"      -m "$( get_brightness    )"
        ;;
        "${MQTT_TOPIC}/contrast/set *")
            set_contrast "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/contrast"        -m "$( get_contrast      )"
        ;;
        "${MQTT_TOPIC}/contrast")
           mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/contrast"         -m "$( get_contrast      )"
        ;;
        "${MQTT_TOPIC}/hue/set *")
            set_hue "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/hue"             -m "$( get_hue           )"
        ;;
        "${MQTT_TOPIC}/hue")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/hue"             -m "$( get_hue           )"
        ;;
        "${MQTT_TOPIC}/saturation/set *")
            set_saturation "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/saturation"      -m "$( get_saturation    )"
        ;;
        "${MQTT_TOPIC}/saturation")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/saturation"      -m "$( get_saturation    )"
        ;;
        "${MQTT_TOPIC}/denoise/set *")
            set_denoise "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/denoise"         -m "$( get_denoise       )"
        ;;
        "${MQTT_TOPIC}/denoise")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/denoise"         -m "$( get_denoise       )"
        ;;
        "${MQTT_TOPIC}/sharpness/set *")
            set_sharpness "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/sharpness"       -m "$( get_sharpness     )"
        ;;
        "${MQTT_TOPIC}/sharpness")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/sharpness"       -m "$( get_sharpness     )"
        ;;
        "${MQTT_TOPIC}/rtsp/set on")
            /etc/init/S99rtsp start
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/rtsp"            -m "$( get_daemon_state rtspd )"
        ;;
        "${MQTT_TOPIC}/rtsp/set off")
            /etc/init/S99rtsp stop
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/rtsp"            -m "$( get_daemon_state rtspd )"
        ;;
        "${MQTT_TOPIC}/rtsp")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/rtsp"            -m "$( get_daemon_state rtspd )"
        ;;
        "${MQTT_TOPIC}/system")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/system"          -m "$( system_status     )"
        ;;
        "${MQTT_TOPIC}/night_mode/set on")
            night_mode on
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/night_mode"      -m "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/set off")
            night_mode off
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/night_mode"      -m "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/night_mode"      -m "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/auto/set on")
            set_basecfg "AUTO_NIGHT_MODE" 1
            /etc/init/S99auto_night_mode start
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/night_mode/auto" -m "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/auto/set off")
            set_basecfg "AUTO_NIGHT_MODE" 0
            /etc/init/S99auto_night_mode stop
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/night_mode/auto" -m "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/auto")
            mosquitto_pub ${PUB_OPTIONS} -t "${MQTT_TOPIC}/night_mode/auto" -m "$( night_mode status )"
        ;;

    esac
done
