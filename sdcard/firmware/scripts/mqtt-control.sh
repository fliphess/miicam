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

if [ "$MQTT_ENABLED" -ne 1 ]
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


/usr/bin/mosquitto_sub ${SUB_OPTIONS} -t "${TOPIC}/#" | while read -r LINE
do
    case $LINE in
        "${TOPIC}/leds/blue/set ON")
            blue_led on
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/blue"       -m "$( blue_led status   )"
        ;;
        "${TOPIC}/leds/blue/set OFF")
            blue_led off
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/blue"       -m "$( blue_led status   )"
        ;;
        "${TOPIC}/leds/blue")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/blue"       -m "$( blue_led status   )"
        ;;


        "${TOPIC}/leds/yellow/set ON")
            yellow_led on
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/yellow"     -m "$( yellow_led status )"
        ;;
        "${TOPIC}/leds/yellow/set OFF")
            yellow_led off
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/yellow"     -m "$( yellow_led status )"
        ;;
        "${TOPIC}/leds/yellow")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/yellow"     -m "$( yellow_led status )"
        ;;


        "${TOPIC}/leds/ir/set ON")
            ir_led on
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/ir"         -m "$( ir_led status     )"
        ;;
        "${TOPIC}/leds/ir/set OFF")
            ir_led off
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/ir"         -m "$( ir_led status     )"
        ;;
        "${TOPIC}/leds/ir")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/leds/ir"         -m "$( ir_led status     )"
        ;;


        "${TOPIC}/ir_cut/set ON")
            ir_cut on
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/ir_cut"          -m "$( ir_cut status     )"
        ;;
        "${TOPIC}/ir_cut/set OFF")
            ir_cut off
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/ir_cut"          -m "$( ir_cut status     )"
        ;;
        "${TOPIC}/ir_cut")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/ir_cut"          -m "$( ir_cut status     )"
        ;;


        "${TOPIC}/brightness/set *")
            set_brightness "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/brightness"      -m "$( get_brightness    )"
        ;;
        "${TOPIC}/brightness")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/brightness"      -m "$( get_brightness    )"
        ;;


        "${TOPIC}/contrast/set *")
            set_contrast "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/contrast"        -m "$( get_contrast      )"
        ;;
        "${TOPIC}/contrast")
           mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/contrast"         -m "$( get_contrast      )"
        ;;


        "${TOPIC}/hue/set *")
            set_hue "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/hue"             -m "$( get_hue           )"
        ;;
        "${TOPIC}/hue")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/hue"             -m "$( get_hue           )"
        ;;


        "${TOPIC}/saturation/set *")
            set_saturation "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/saturation"      -m "$( get_saturation    )"
        ;;
        "${TOPIC}/saturation")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/saturation"      -m "$( get_saturation    )"
        ;;


        "${TOPIC}/denoise/set *")
            set_denoise "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/denoise"         -m "$( get_denoise       )"
        ;;
        "${TOPIC}/denoise")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/denoise"         -m "$( get_denoise       )"
        ;;


        "${TOPIC}/sharpness/set *")
            set_sharpness "$( get_value "$LINE" )"
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/sharpness"       -m "$( get_sharpness     )"
        ;;
        "${TOPIC}/sharpness")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/sharpness"       -m "$( get_sharpness     )"
        ;;


        "${TOPIC}/rtsp/set ON")
            /etc/init/S99rtsp start
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/rtsp"            -m "$( get_daemon_state rtspd )"
        ;;
        "${TOPIC}/rtsp/set OFF")
            /etc/init/S99rtsp stop
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/rtsp"            -m "$( get_daemon_state rtspd )"
        ;;
        "${TOPIC}/rtsp")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/rtsp"            -m "$( get_daemon_state rtspd )"
        ;;


        "${TOPIC}/system")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/system"          -m "$( system_status     )"
        ;;


        "${TOPIC}/night_mode/set ON")
            night_mode on
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/night_mode"      -m "$( night_mode status )"
        ;;
        "${TOPIC}/night_mode/set OFF")
            night_mode off
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/night_mode"      -m "$( night_mode status )"
        ;;
        "${TOPIC}/night_mode")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/night_mode"      -m "$( night_mode status )"
        ;;


        "${TOPIC}/night_mode/auto/set ON")
            set_basecfg "AUTO_NIGHT_MODE" 1
            /etc/init/S99auto_night_mode start
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/night_mode/auto" -m "$( night_mode status )"
        ;;
        "${TOPIC}/night_mode/auto/set OFF")
            set_basecfg "AUTO_NIGHT_MODE" 0
            /etc/init/S99auto_night_mode stop
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/night_mode/auto" -m "$( night_mode status )"
        ;;
        "${TOPIC}/night_mode/auto")
            mosquitto_pub ${PUB_OPTIONS} -t "${TOPIC}/night_mode/auto" -m "$( night_mode status )"
        ;;

    esac
done
