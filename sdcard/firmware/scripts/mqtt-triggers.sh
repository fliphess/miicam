################################################################################
## Functions that are triggered on incoming mqtt commands                     ##
################################################################################

########################################
## Load functions if not yet sourced  ##
########################################

if [ -z "${SD_MOUNTDIR}" ] || [ "${FUNCTIONS_SOURCED}" -ne 1 ]
then
    SD_MOUNTDIR="/tmp/sd"

    if [ -r "${SD_MOUNTDIR}/firmware/scripts/functions.sh" ]
    then
        . "${SD_MOUNTDIR}/firmware/scripts/functions.sh"
    else
        echo "Unable to load basic functions"
        exit 1
    fi
fi

########################################
## Variables                          ##
########################################

## Logger function
log() {
    echo "$( date ) - ${*}" | tee -a ${LOGFILE}
}


## Get value parameter from input string
get_value() {
    local INPUT="${1}"
    local TOPIC="$( echo ${INPUT} | cut -d " " -f 1 )"
    echo ${INPUT} | sed -e "s#${TOPIC}##g"
}


## Get topic value parameter from input string
get_topic() {
    local INPUT="${1}"
    echo ${INPUT} | cut -d " " -f 1
}


########################################
## Triggered on led/blue/set          ##
########################################

led_blue_status()
{
    mqtt_send "${MQTT_TOPIC}/leds/blue" "$( blue_led status )"
}

led_blue_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
        log "MQTT Action: Turning blue led on"
        blue_led on
        mqtt_send "${MQTT_TOPIC}/leds/blue" "${MQTT_ON}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Turning blue led off"
        blue_led off
        mqtt_send "${MQTT_TOPIC}/leds/blue" "${MQTT_OFF}"
    fi
}


########################################
## Triggered on led/yellow/set        ##
########################################

led_yellow_status()
{
     mqtt_send "${MQTT_TOPIC}/leds/yellow" "$( yellow_led status )"
}

led_yellow_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
        log "MQTT Action: Turning yellow led on"
        yellow_led on
        mqtt_send "${MQTT_TOPIC}/leds/yellow" "${MQTT_ON}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Turning yellow led off"
        yellow_led off
        mqtt_send "${MQTT_TOPIC}/leds/yellow" "${MQTT_OFF}"
    fi
}


########################################
## Triggered on led/ir/set            ##
########################################

led_ir_status()
{
    mqtt_send "${MQTT_TOPIC}/leds/ir"      "$( /tmp/sd/firmware/bin/ir_led -s | last_f )"
    mqtt_send "${MQTT_TOPIC}/leds/ir/json" "$( /tmp/sd/firmware/bin/ir_led -j )"
}

led_ir_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
        log "MQTT Action: Turning ir led on"
        ir_led on
        mqtt_send "${MQTT_TOPIC}/leds/ir" "${MQTT_ON}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Turning ir led off"
        ir_led off
        mqtt_send "${MQTT_TOPIC}/leds/ir" "${MQTT_OFF}"
    fi
}

########################################
## Triggered on ir_cut/set            ##
########################################

ir_cut_status()
{
     mqtt_send "${MQTT_TOPIC}/ir_cut"      "$( /tmp/sd/firmware/bin/ir_cut -s | last_f )"
     mqtt_send "${MQTT_TOPIC}/ir_cut/json" "$( /tmp/sd/firmware/bin/ir_cut -j )"
}

ir_cut_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
        log "MQTT Action: Turning IR cut to on"
        /tmp/sd/firmware/bin/ir_cut -e
        mqtt_send "${MQTT_TOPIC}/ir_cut" "${MQTT_ON}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
    log "MQTT Action: Turning IR cut to off"
        /tmp/sd/firmware/bin/ir_cut -d
        mqtt_send "${MQTT_TOPIC}/ir_cut" "${MQTT_OFF}"
    fi
}


########################################
## Triggered on mirror_mode/set       ##
########################################

mirror_mode_status()
{
     mqtt_send "${MQTT_TOPIC}/mirror" "$( /tmp/sd/firmware/bin/mirrormode -s | last_f )"
}


mirror_mode_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
    log "MQTT Action: Setting mirror mode to ${VALUE}"
        /tmp/sd/firmware/bin/mirrormode -e
        mqtt_send "${MQTT_TOPIC}/mirror_mode" "${MQTT_ON}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Setting mirror mode to ${VALUE}"
        /tmp/sd/firmware/bin/mirrormode -d
        mqtt_send "${MQTT_TOPIC}/mirror_mode" "${MQTT_OFF}"
    fi
}


########################################
## Triggered on night_mode/set        ##
########################################

night_mode_status()
{
    mqtt_send "${MQTT_TOPIC}/night_mode"      "$( /tmp/sd/firmware/bin/nightmode -s | last_f )"
    mqtt_send "${MQTT_TOPIC}/night_mode/json" "$( /tmp/sd/firmware/bin/nightmode -j )"
}

night_mode_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
        log "MQTT Action: Setting night mode to ${VALUE}"
        /tmp/sd/firmware/bin/nightmode -e
        mqtt_send "${MQTT_TOPIC}/night_mode" "${MQTT_OFF}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Setting night mode to ${VALUE}"
        /tmp/sd/firmware/bin/nightmode -d
        mqtt_send "${MQTT_TOPIC}/night_mode" "${MQTT_OFF}"
    fi
}

########################################
## Triggered on flip_mode/set         ##
########################################

flip_mode_status()
{
    mqtt_send "${MQTT_TOPIC}/flip_mode" "$( /tmp/sd/firmware/bin/flipmode -s | last_f )"
}

flip_mode_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
        log "MQTT Action: Setting flip mode to ${VALUE}"
        /tmp/sd/firmware/bin/flipmode -e
        mqtt_send "${MQTT_TOPIC}/flip_mode" "${MQTT_ON}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Setting flip mode to ${VALUE}"
        /tmp/sd/firmware/bin/flipmode -d
        mqtt_send "${MQTT_TOPIC}/flip_mode" "${MQTT_OFF}"
    fi
}

########################################
## Triggered on brightness/set        ##
########################################

brightness_status()
{
    mqtt_send "${MQTT_TOPIC}/brightness" "$( /tmp/sd/firmware/bin/camera_adjust -t brightness -g | last_f )"
}

brightness_set()
{
    local VALUE="${1}"

    log "MQTT Action: Setting brightness to ${VALUE}"
    /tmp/sd/firmware/bin/camera_adjust -t brightness -s "${VALUE}"

    brightness_status
}

########################################
## Triggered on contrast/set          ##
########################################

contrast_status()
{
    mqtt_send "${MQTT_TOPIC}/contrast" "$( /tmp/sd/firmware/bin/camera_adjust -t contrast -g | last_f )"
}

contrast_set()
{
    local VALUE="${1}"

    log "MQTT Action: Setting contrast to ${VALUE}"
    /tmp/sd/firmware/bin/camera_adjust -t contrast -s "${VALUE}"

    contrast_status
}

########################################
## Triggered on hue/set               ##
########################################

hue_status()
{
    mqtt_send "${MQTT_TOPIC}/hue" "$( /tmp/sd/firmware/bin/camera_adjust -t hue -g | last_f )"
}

hue_set()
{
    local VALUE="${1}"

    log "MQTT Action: Setting hue to ${VALUE}"
    /tmp/sd/firmware/bin/camera_adjust -t hue -s "${VALUE}"

    hue_status
}

########################################
## Triggered on saturation/set        ##
########################################

saturation_status()
{
    mqtt_send "${MQTT_TOPIC}/saturation" "$( /tmp/sd/firmware/bin/camera_adjust -t saturation -g | last_f )"
}

saturation_set()
{
    local VALUE="${1}"

    log "MQTT Action: Setting saturation to ${VALUE}"
    /tmp/sd/firmware/bin/camera_adjust -t saturation -s "${VALUE}"

    saturation_set
}

########################################
## Triggered on denoise/set           ##
########################################

denoise_status()
{
    mqtt_send "${MQTT_TOPIC}/denoise" "$( /tmp/sd/firmware/bin/camera_adjust -t denoise -g | last_f )"
}

denoise_set()
{
    local VALUE="${1}"

    log "MQTT Action: Setting denoise to ${VALUE}"
    /tmp/sd/firmware/bin/camera_adjust -t denoise -s "${VALUE}"

    denoise_status
}

########################################
## Triggered on sharpness/set         ##
########################################

sharpness_status()
{
    mqtt_send "${MQTT_TOPIC}/sharpness" "$( /tmp/sd/firmware/bin/camera_adjust -t sharpness -g | last_f )"
}

sharpness_set()
{
    local VALUE="${1}"

    log "MQTT Action: Setting sharpness to ${VALUE}"
    /tmp/sd/firmware/bin/camera_adjust -t sharpness -s "${VALUE}"

    sharpness_status
}

########################################
## Triggered on rtsp/set              ##
########################################

rtsp_status()
{
    mqtt_send "${MQTT_TOPIC}/rtsp" "$( get_daemon_state rtspd )"
}

rtsp_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_ON}" ]
    then
        log "MQTT Action: Turning on rtspd"
        /etc/init/S99rtsp start
        mqtt_send "${MQTT_TOPIC}/rtsp" "${MQTT_ON}"

    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Turning off rtspd"
        /etc/init/S99rtsp stop
        mqtt_send "${MQTT_TOPIC}/rtsp" "${MQTT_OFF}"
    fi
}

########################################
## Triggered on night_mode/auto/set   ##
########################################

night_mode_auto_status()
{
    mqtt_send "${MQTT_TOPIC}/night_mode/auto" "$( night_mode status )"
}

night_mode_auto_set()
{
    local VALUE="${1}"

    if [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Starting auto night mode daemon"
        /etc/init/S99auto_night_mode start
        mqtt_send "${MQTT_TOPIC}/night_mode/auto" "${MQTT_ON}"
    elif [ "${VALUE}" == "${MQTT_OFF}" ]
    then
        log "MQTT Action: Stopping auto night mode daemon"
        /etc/init/S99auto_night_mode stop
        mqtt_send "${MQTT_TOPIC}/night_mode/auto" "${MQTT_OFF}"
    fi
}

########################################
## Triggered on system                ##
########################################

get_system_status()
{
    log "MQTT Action: Sending system status"
    mqtt_send "${MQTT_TOPIC}/system" "$( system_status --json )"
}

get_mode_status()
{
    log "MQTT Action: Sending isp328 mode status"
    mqtt_send "${MQTT_TOPIC}/camera/modes" "$( mode_status --json )"
}

get_led_status()
{
    log "MQTT Action: Sending led status"
    mqtt_send "${MQTT_TOPIC}/leds/status" "$( led_status --json )"
}

get_camera_status()
{
    log "MQTT Action: Sending camera settings"
    mqtt_send "${MQTT_TOPIC}/camera/settings" "$( /tmp/sd/firmware/bin/camera_adjust -j )"
}


########################################
## Triggered on snapshot              ##
########################################

snapshot_get()
{
    log "MQTT Action: Requesting video recording"
    mqtt_send "${MQTT_TOPIC}/recording" "$( /tmp/sd/firmware/bin/take_video 2> /dev/null | last_f )"
}


########################################
## Triggered on recording             ##
########################################

recording_get()
{
    log "MQTT Action: Requesting snapshot"
    mqtt_send "${MQTT_TOPIC}/snapshot" "$( /tmp/sd/firmware/bin/take_snapshot 2> /dev/null | last_f )"
}



