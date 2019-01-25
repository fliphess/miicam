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
## Kill all hanging mosquitto_sub processes
########################################

killall mosquitto_sub 2>/dev/null


########################################
## Overrule logfile
########################################

LOGFILE="${SD_MOUNTDIR}/log/mqtt.log"


########################################
## The main loop                      ##
########################################

while :
do
    mqtt_subscribe "${MQTT_TOPIC}/#" "ctl" | while read -r LINE
    do
        log "Incoming MQTT message: ${LINE}"

        ## Get value from line
        VALUE="$( get_value "${LINE}" )"
        TOPIC="$( get_topic "${LINE}" )"

        case "${TOPIC}" in
            "${MQTT_TOPIC}/leds/blue/set")
                led_blue_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/leds/blue")
                led_blue_status
            ;;

            "${MQTT_TOPIC}/leds/yellow/set")
                led_yellow_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/leds/yellow")
                led_yellow_status
            ;;

            "${MQTT_TOPIC}/leds/ir/set")
                led_ir_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/leds/ir")
                led_ir_status
            ;;

            "${MQTT_TOPIC}/ir_cut/set")
                ir_cut_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/ir_cut")
                ir_cut_status
            ;;

            "${MQTT_TOPIC}/mirror_mode/set")
                mirror_mode_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/mirror_mode")
                mirror_mode_status
            ;;

            "${MQTT_TOPIC}/night_mode/set")
                night_mode_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/night_mode")
                night_mode_status
            ;;

            "${MQTT_TOPIC}/flip_mode/set")
                flip_mode_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/flip_mode")
                flip_mode_status
            ;;

            "${MQTT_TOPIC}/brightness/set")
                brightness_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/brightness")
                brightness_status
            ;;

            "${MQTT_TOPIC}/contrast/set")
                contrast_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/contrast")
                contrast_status
            ;;

            "${MQTT_TOPIC}/hue/set")
                hue_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/hue")
                hue_status
            ;;

            "${MQTT_TOPIC}/saturation/set")
                saturation_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/saturation")
                saturation_status
            ;;

            "${MQTT_TOPIC}/denoise/set")
                denoise_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/denoise")
                denoise_status
            ;;

            "${MQTT_TOPIC}/sharpness/set")
                sharpness_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/sharpness")
                sharpness_status
            ;;

            "${MQTT_TOPIC}/rtsp/set")
                rtsp_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/rtsp")
                rtsp_status
            ;;

            "${MQTT_TOPIC}/night_mode/auto/set")
                night_mode_auto_set "${VALUE}"
            ;;
            "${MQTT_TOPIC}/night_mode/auto")
                night_mode_auto_status
            ;;

            "${MQTT_TOPIC}/system")
                get_system_status
            ;;
            "${MQTT_TOPIC}/leds/status")
                get_led_status
            ;;
            "${MQTT_TOPIC}/camera/modes")
                get_mode_status
            ;;
            "${MQTT_TOPIC}/camera")
                get_camera_status
            ;;

            "${MQTT_TOPIC}/recording")
                recording_get
            ;;
            "${MQTT_TOPIC}/snapshot")
                snapshot_get
            ;;
        esac
    done
done
