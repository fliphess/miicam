#!/bin/sh

## Set scriptname
SCRIPT="$( basename ${0} )"

## Set mountdir
SD_MOUNTDIR="/tmp/sd"

## Load functions
if [ -r "${SD_MOUNTDIR}/firmware/scripts/functions.sh" ]
then
    . "${SD_MOUNTDIR}/firmware/scripts/functions.sh"
else
    echo "Unable to load basic functions"
    exit 1
fi

## Bail out if MQTT is disabled in config.cfg
if [ "${ENABLE_MQTT}" -ne 1 ]
then
    die "MQTT Support is disabled in the configuration."
fi

## Kill all hanging mosquitto_sub processes
killall mosquitto_sub 2>/dev/null

## Overrule logfile
LOGFILE="${SD_MOUNTDIR}/log/mqtt.log"

## Logger function
log() {
    echo "$( date ) - ${*}" | tee -a ${LOGFILE}
}

## Get value parameter from input string
get_value() {
    INPUT="${1}"
    TOPIC="$( echo ${INPUT} | cut -d " " -f 1 )"
    echo ${INPUT} | sed -e "s#${TOPIC}##g"
}

## Get topic value parameter from input string
get_topic() {
    INPUT="${1}"
    echo ${INPUT} | cut -d " " -f 1
}


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
                log "MQTT Action: Turning blue led to ${VALUE}"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    blue_led on
                    mqtt_send "${MQTT_TOPIC}/leds/blue" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    blue_led off
                    mqtt_send "${MQTT_TOPIC}/leds/blue" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/leds/blue")
                mqtt_send "${MQTT_TOPIC}/leds/blue" "$( blue_led status )"
            ;;
            "${MQTT_TOPIC}/leds/yellow/set")
                log "MQTT Action: Turning yellow to ${VALUE}"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    yellow_led on
                    mqtt_send "${MQTT_TOPIC}/leds/yellow" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    yellow_led off
                    mqtt_send "${MQTT_TOPIC}/leds/yellow" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/leds/yellow")
                mqtt_send "${MQTT_TOPIC}/leds/yellow" "$( yellow_led status )"
            ;;
            "${MQTT_TOPIC}/leds/ir/set")
                log "MQTT Action: Turning IR led to ${VALUE}"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    /tmp/sd/firmware/bin/ir_led -e
                    mqtt_send "${MQTT_TOPIC}/leds/ir" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /tmp/sd/firmware/bin/ir_led -d
                    mqtt_send "${MQTT_TOPIC}/leds/ir" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/leds/ir")
                mqtt_send "${MQTT_TOPIC}/leds/ir"      "$( /tmp/sd/firmware/bin/ir_led -s | awk '{print $NF}' )"
                mqtt_send "${MQTT_TOPIC}/leds/ir/info" "$( /tmp/sd/firmware/bin/ir_led -j )"
            ;;

            "${MQTT_TOPIC}/ir_cut/set")
                log "MQTT Action: Turning IR cut to ${VALUE}"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    /tmp/sd/firmware/bin/ir_cut -e
                    mqtt_send "${MQTT_TOPIC}/ir_cut" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /tmp/sd/firmware/bin/ir_cut -d
                    mqtt_send "${MQTT_TOPIC}/ir_cut" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/ir_cut")
                mqtt_send "${MQTT_TOPIC}/ir_cut" "$( /tmp/sd/firmware/bin/ir_cut -s | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/mirror_mode/set")
                log "MQTT Action: Setting mirror mode to ${VALUE}"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    /tmp/sd/firmware/bin/mirrormode -e
                    mqtt_send "${MQTT_TOPIC}/mirror_mode" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /tmp/sd/firmware/bin/mirrormode -d
                    mqtt_send "${MQTT_TOPIC}/mirror_mode" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/mirror_mode")
                mqtt_send "${MQTT_TOPIC}/mirror" "$( /tmp/sd/firmware/bin/mirrormode -s | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/night_mode/set")
                log "MQTT Action: Setting night mode to ${VALUE}"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    /tmp/sd/firmware/bin/nightmode -e
                    mqtt_send "${MQTT_TOPIC}/night_mode" "${MQTT_OFF}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /tmp/sd/firmware/bin/nightmode -d
                    mqtt_send "${MQTT_TOPIC}/night_mode" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/night_mode")
                mqtt_send "${MQTT_TOPIC}/night_mode" "$( /tmp/sd/firmware/bin/nightmode -s | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/flip_mode/set")
                log "MQTT Action: Setting flip mode to ${VALUE}"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    /tmp/sd/firmware/bin/flipmode -e
                    mqtt_send "${MQTT_TOPIC}/flip_mode" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /tmp/sd/firmware/bin/flipmode -d
                    mqtt_send "${MQTT_TOPIC}/flip_mode" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/flip_mode")
                mqtt_send "${MQTT_TOPIC}/flip_mode" "$( /tmp/sd/firmware/bin/flipmode -s | awk '{print $NF}' )"
            ;;

            "${MQTT_TOPIC}/brightness/set")
                log "MQTT Action: Setting brightness to ${VALUE}"
                /tmp/sd/firmware/bin/camera_adjust -t brightness -s "${VALUE}"
                mqtt_send "${MQTT_TOPIC}/brightness" "$( /tmp/sd/firmware/bin/camera_adjust -t brightness -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/brightness")
                mqtt_send "${MQTT_TOPIC}/brightness" "$( /tmp/sd/firmware/bin/camera_adjust -t brightness -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/contrast/set")
                log "MQTT Action: Setting contrast to ${VALUE}"
                /tmp/sd/firmware/bin/camera_adjust -t contrast -s "${VALUE}"
                mqtt_send "${MQTT_TOPIC}/contrast" "$( /tmp/sd/firmware/bin/camera_adjust -t contrast -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/contrast")
                mqtt_send "${MQTT_TOPIC}/contrast" "$( /tmp/sd/firmware/bin/camera_adjust -t contrast -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/hue/set")
                log "MQTT Action: Setting hue to ${VALUE}"
                /tmp/sd/firmware/bin/camera_adjust -t hue -s "${VALUE}"
                mqtt_send "${MQTT_TOPIC}/hue" "$( /tmp/sd/firmware/bin/camera_adjust -t hue -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/hue")
                mqtt_send "${MQTT_TOPIC}/hue" "$( /tmp/sd/firmware/bin/camera_adjust -t hue -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/saturation/set")
                log "MQTT Action: Setting saturation to ${VALUE}"
                /tmp/sd/firmware/bin/camera_adjust -t saturation -s "${VALUE}"
                mqtt_send "${MQTT_TOPIC}/saturation" "$( /tmp/sd/firmware/bin/camera_adjust -t saturation -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/saturation")
                mqtt_send "${MQTT_TOPIC}/saturation" "$( /tmp/sd/firmware/bin/camera_adjust -t saturation -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/denoise/set")
                log "MQTT Action: Setting denoise to ${VALUE}"
                /tmp/sd/firmware/bin/camera_adjust -t denoise -s "${VALUE}"
                mqtt_send "${MQTT_TOPIC}/denoise" "$( /tmp/sd/firmware/bin/camera_adjust -t denoise -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/denoise")
                mqtt_send "${MQTT_TOPIC}/denoise" "$( /tmp/sd/firmware/bin/camera_adjust -t denoise -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/sharpness/set")
                log "MQTT Action: Setting sharpness to ${VALUE}"
                /tmp/sd/firmware/bin/camera_adjust -t sharpness -s "${VALUE}"
                mqtt_send "${MQTT_TOPIC}/sharpness" "$( /tmp/sd/firmware/bin/camera_adjust -t sharpness -g | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/sharpness")
                mqtt_send "${MQTT_TOPIC}/sharpness" "$( /tmp/sd/firmware/bin/camera_adjust -t sharpness -g | awk '{print $NF}' )"
            ;;

            "${MQTT_TOPIC}/rtsp/set")
                log "MQTT Action: Turning on rtspd"
                if [ "${VALUE}" == "${MQTT_ON}" ]
                then
                    /etc/init/S99rtsp start
                    mqtt_send "${MQTT_TOPIC}/rtsp" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /etc/init/S99rtsp stop
                    mqtt_send "${MQTT_TOPIC}/rtsp" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/rtsp")
                mqtt_send "${MQTT_TOPIC}/rtsp" "$( get_daemon_state rtspd )"
            ;;

            "${MQTT_TOPIC}/night_mode/auto/set")
                log "MQTT Action: Starting auto night mode daemon"
                if [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /etc/init/S99auto_night_mode start
                    mqtt_send "${MQTT_TOPIC}/night_mode/auto" "${MQTT_ON}"
                elif [ "${VALUE}" == "${MQTT_OFF}" ]
                then
                    /etc/init/S99auto_night_mode stop
                    mqtt_send "${MQTT_TOPIC}/night_mode/auto" "${MQTT_OFF}"
                fi
            ;;
            "${MQTT_TOPIC}/night_mode/auto")
                mqtt_send "${MQTT_TOPIC}/night_mode/auto" "$( night_mode status )"
            ;;

            "${MQTT_TOPIC}/system")
                mqtt_send "${MQTT_TOPIC}/system" "$( system_status )"
            ;;
            "${MQTT_TOPIC}/recording")
                mqtt_send "${MQTT_TOPIC}/recording" "$( /tmp/sd/firmware/bin/take_video 2> /dev/null | awk '{print $NF}' )"
            ;;
            "${MQTT_TOPIC}/snapshot")
                mqtt_send "${MQTT_TOPIC}/snapshot" "$( /tmp/sd/firmware/bin/take_snapshot 2> /dev/null | awk '{print $NF}' )"
            ;;
        esac
    done
done
