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

## Kill all hanging mosquitto_sub processes
killall mosquitto_sub 2>/dev/null


get_value() {
  INPUT="$1"
  TOPIC="$( echo $INPUT | cut -d " " -f 1 )"

  echo $INPUT | sed -e "s#$TOPIC##g"
}


mqtt_subscribe "${MQTT_TOPIC}/#" | while read -r LINE
do
    case $LINE in
        "${MQTT_TOPIC}/leds/blue/set on")
            blue_led on
            mqtt_send "${MQTT_TOPIC}/leds/blue"       "$( blue_led status )"
        ;;
        "${MQTT_TOPIC}/leds/blue/set off")
            blue_led off
            mqtt_send "${MQTT_TOPIC}/leds/blue"       "$( blue_led status )"
        ;;
        "${MQTT_TOPIC}/leds/blue")
            mqtt_send "${MQTT_TOPIC}/leds/blue"       "$( blue_led status )"
        ;;
        "${MQTT_TOPIC}/leds/yellow/set on")
            yellow_led on
            mqtt_send "${MQTT_TOPIC}/leds/yellow"     "$( yellow_led status )"
        ;;
        "${MQTT_TOPIC}/leds/yellow/set off")
            yellow_led off
            mqtt_send "${MQTT_TOPIC}/leds/yellow"     "$( yellow_led status )"
        ;;
        "${MQTT_TOPIC}/leds/yellow")
            mqtt_send "${MQTT_TOPIC}/leds/yellow"     "$( yellow_led status )"
        ;;
        "${MQTT_TOPIC}/leds/ir/set on")
            ir_led on
            mqtt_send "${MQTT_TOPIC}/leds/ir"         "$( ir_led status )"
        ;;
        "${MQTT_TOPIC}/leds/ir/set off")
            ir_led off
            mqtt_send "${MQTT_TOPIC}/leds/ir"         "$( ir_led status )"
        ;;
        "${MQTT_TOPIC}/leds/ir")
            mqtt_send "${MQTT_TOPIC}/leds/ir"         "$( ir_led status )"
        ;;
        "${MQTT_TOPIC}/ir_cut/set on")
            ir_cut on
            mqtt_send "${MQTT_TOPIC}/ir_cut"          "$( ir_cut status )"
        ;;
        "${MQTT_TOPIC}/ir_cut/set off")
            ir_cut off
            mqtt_send "${MQTT_TOPIC}/ir_cut"          "$( ir_cut status )"
        ;;
        "${MQTT_TOPIC}/ir_cut")
            mqtt_send "${MQTT_TOPIC}/ir_cut"          "$( ir_cut status )"
        ;;
        "${MQTT_TOPIC}/brightness/set *")
            set_brightness "$( get_value "$LINE" )"
            mqtt_send "${MQTT_TOPIC}/brightness"      "$( get_brightness )"
        ;;
        "${MQTT_TOPIC}/brightness")
            mqtt_send "${MQTT_TOPIC}/brightness"      "$( get_brightness )"
        ;;
        "${MQTT_TOPIC}/contrast/set *")
            set_contrast "$( get_value "$LINE" )"
            mqtt_send "${MQTT_TOPIC}/contrast"        "$( get_contrast )"
        ;;
        "${MQTT_TOPIC}/contrast")
           mqtt_send "${MQTT_TOPIC}/contrast"         "$( get_contrast )"
        ;;
        "${MQTT_TOPIC}/hue/set *")
            set_hue "$( get_value "$LINE" )"
            mqtt_send "${MQTT_TOPIC}/hue"             "$( get_hue )"
        ;;
        "${MQTT_TOPIC}/hue")
            mqtt_send "${MQTT_TOPIC}/hue"             "$( get_hue )"
        ;;
        "${MQTT_TOPIC}/saturation/set *")
            set_saturation "$( get_value "$LINE" )"
            mqtt_send "${MQTT_TOPIC}/saturation"      "$( get_saturation )"
        ;;
        "${MQTT_TOPIC}/saturation")
            mqtt_send "${MQTT_TOPIC}/saturation"      "$( get_saturation )"
        ;;
        "${MQTT_TOPIC}/denoise/set *")
            set_denoise "$( get_value "$LINE" )"
            mqtt_send "${MQTT_TOPIC}/denoise"         "$( get_denoise )"
        ;;
        "${MQTT_TOPIC}/denoise")
            mqtt_send "${MQTT_TOPIC}/denoise"         "$( get_denoise )"
        ;;
        "${MQTT_TOPIC}/sharpness/set *")
            set_sharpness "$( get_value "$LINE" )"
            mqtt_send "${MQTT_TOPIC}/sharpness"       "$( get_sharpness )"
        ;;
        "${MQTT_TOPIC}/sharpness")
            mqtt_send "${MQTT_TOPIC}/sharpness"       "$( get_sharpness     )"
        ;;
        "${MQTT_TOPIC}/rtsp/set on")
            /etc/init/S99rtsp start
            mqtt_send "${MQTT_TOPIC}/rtsp"            "$( get_daemon_state rtspd )"
        ;;
        "${MQTT_TOPIC}/rtsp/set off")
            /etc/init/S99rtsp stop
            mqtt_send "${MQTT_TOPIC}/rtsp"            "$( get_daemon_state rtspd )"
        ;;
        "${MQTT_TOPIC}/rtsp")
            mqtt_send "${MQTT_TOPIC}/rtsp"            "$( get_daemon_state rtspd )"
        ;;
        "${MQTT_TOPIC}/system")
            mqtt_send "${MQTT_TOPIC}/system"          "$( system_status )"
        ;;
        "${MQTT_TOPIC}/night_mode/set on")
            night_mode on
            mqtt_send "${MQTT_TOPIC}/night_mode"      "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/set off")
            night_mode off
            mqtt_send "${MQTT_TOPIC}/night_mode"      "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode")
            mqtt_send "${MQTT_TOPIC}/night_mode"      "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/auto/set on")
            set_basecfg "AUTO_NIGHT_MODE" 1
            /etc/init/S99auto_night_mode start
            mqtt_send "${MQTT_TOPIC}/night_mode/auto" "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/auto/set off")
            set_basecfg "AUTO_NIGHT_MODE" 0
            /etc/init/S99auto_night_mode stop
            mqtt_send "${MQTT_TOPIC}/night_mode/auto" "$( night_mode status )"
        ;;
        "${MQTT_TOPIC}/night_mode/auto")
            mqtt_send "${MQTT_TOPIC}/night_mode/auto" "$( night_mode status )"
        ;;

    esac
done
