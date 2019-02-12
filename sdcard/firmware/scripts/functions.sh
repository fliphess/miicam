#!/bin/sh

##################################################################################
## purpose: standard library                                                    ##
## license: GPLv3+, http://www.gnu.org/licenses/gpl-3.0.html                    ##
## author: Jan Sperling, 2018                                                   ##
##################################################################################

SD_MOUNTDIR="/tmp/sd"

LOGDIR="${SD_MOUNTDIR}/log"
LOGFILE="${LOGDIR}/ft_boot.log"
BASECFG="${SD_MOUNTDIR}/config.cfg"
CTRL="${SD_MOUNTDIR}/firmware/bin/chuangmi_ctrl"

if [ -f "${BASECFG}" ]
then
    . "${BASECFG}"
fi

## Print the last field
alias last_f="awk '{print \$NF}'"


##################################################################################
## System Functions                                                             ##
##################################################################################

## Logger function
log()
{
    echo "$( date ) - $*" | tee -a $LOGFILE
}


## Exit with error function
die()
{
    log "ERROR - $1"
    exit 1
}

wait_for_network_until()
{
    RETRIES="$1"
    SLEEP="$2"
    IP="$3"

    if [ "x$IP" == "x" ]
    then
        IP="8.8.8.8"
    fi

    if [ "x${RETRIES}" == "x" ]
    then
        RETRIES=10
    fi

    if [ "x$SLEEP" == "x" ]
    then
        SLEEP=5
    fi

    printf "*** Waiting for ping reply from $IP for $RETRIES times..."

    TRIES=0
    until [ $TRIES -ge $RETRIES ]
    do
        ping -c 1 -w 1 "$IP" >/dev/null 2>&1 && break
        TRIES=$(( $TRIES+1 ))
        echo -n "."
        sleep 5
    done

    if [ $TRIES -ge $RETRIES ]
    then
       echo " FAIL"
       echo "*** No internet connection detected! Sad panda :("
       return 1
    else
       echo " OK"
       return 0
    fi
}

get_last_image()
{
    ## Find last created snapshot
    if [ -d /tmp/sd/RECORDED_IMAGES ]
    then
        find /tmp/sd/RECORDED_IMAGES/ -type f | sort -r | tail -n 1
    fi

    return $?
}


get_last_video()
{
    ## Find last created video
    if [ -d /tmp/sd/RECORDED_VIDEOS ]
    then
        find /tmp/sd/RECORDED_VIDEOS/ -type f | sort -r | tail -n 1
    fi

    return $?
}

##################################################################################
## Binary manipulation functions                                                ##
##################################################################################

## Creates /tmp/disable-binary
create_disable_binary()
{

    if [ ! -x /tmp/disable-binary ]
    then
        echo -e '#!/bin/sh\necho "$0 disabled with MiiCam"' > /tmp/disable-binary
        chmod +x /tmp/disable-binary
    fi
}

## Disable binary and optionally delete it from restartd.conf
disable_binary()
{
    BINARY="$1"
    RESTART="$2"

    create_disable_binary

    printf "*** Disabling ${1##*/}... \n"

    if pgrep "${BINARY}" >/dev/null
    then
        pkill -9 "${BINARY}"
    fi

    if ! ( mount | grep -q "${BINARY}" )
    then
        mount --bind /tmp/disable-binary "${BINARY}"
    fi

    # update restartd.conf
    if [ -n "${RESTART}" ] && [ -f /tmp/etc/restartd.conf ] && (grep -q ^"${RESTART} " /tmp/etc/restartd.conf)
    then
        sed -i "/^${RESTART} /d" /tmp/etc/restartd.conf
    fi
}

## Enable binary and optionally add it to restartd.conf
enable_binary()
{
    BINARY="$1"
    RESTART="$2"

    printf "*** Enabling ${1##*/}... \n"

    if ( mount | grep -q "${BINARY}" )
    then
        umount "${BINARY}"
    fi

    # update restartd.conf
    if [ -n "${RESTART}" ] && [ -f /tmp/etc/restartd.conf ] && ! grep -q ^"${RESTART} " /tmp/etc/restartd.conf
    then
        grep ^"${RESTART} " /tmp/etc/restartd.conf.org >> /tmp/etc/restartd.conf
    fi
}


##################################################################################
## Daemon functions                                                             ##
##################################################################################

## Start daemon
start_daemon()
{
    echo "*** Starting ${NAME} ${DESC}... "

    start-stop-daemon --start --quiet --oknodo --exec "${DAEMON}" -- ${DAEMON_OPTS}
    RC="$?"


    return "${RC}"
}

## Start a process as background daemon
start_daemon_background()
{
    echo "*** Starting ${NAME} ${DESC}... "

    start-stop-daemon --start --quiet --oknodo --pidfile "${PIDFILE}" --make-pidfile --background --exec "${DAEMON}" -- ${DAEMON_OPTS}
    RC="$?"

    return "${RC}"
}

## Stop daemon
stop_daemon()
{
    echo "*** Stopping ${NAME} ${DESC} ..."

    start-stop-daemon --stop --quiet --oknodo --pidfile "${PIDFILE}"
    RC="$?"

    return "${RC}"
}

## Stop background daemon
stop_daemon_background()
{
    stop_daemon

    if [ "${RC}" -eq 0 ] && [ -f "${PIDFILE}" ]
    then
        rm "${PIDFILE}"
    fi

    return "${RC}"
}

## Status of a daemon
status_daemon()
{
    PID="$( cat "${PIDFILE}" 2>/dev/null )"

    if [ "${PID}" ]
    then
        if kill -0 "${PID}" >/dev/null 2>/dev/null
        then
            echo "${DESC} is running with PID: ${PID}"
            RC="0"
        else
            echo "${DESC} is dead"
            RC="1"
        fi
    else
        echo "${DESC} is not running"
        RC="3"
    fi

    return "${RC}"
}

## Check for daemon executable
check_daemon_bin()
{
    BINARY="$1"
    DESC="$2"

    [ ! -x "$BINARY" ]  && return 1
    [ "x$DESC" == "x" ] && DESC="$BINARY"

    if [ ! -x "${BINARY}" ]
    then
        echo "Could not find ${DESC} binary"
        exit 1
    fi
}

## Print start-stop-daemon return status
ok_fail()
{
    INPUT="$1"

    if [ "$INPUT" -eq 0 ]
    then
        echo "OK"
    else
        echo "FAIL"
    fi
}


## Status of a daemon
get_daemon_state()
{
    DAEMON="$1"
    PIDFILE="/var/run/$DAEMON"
    PID="$( cat "${PIDFILE}" 2>/dev/null )"
    RC="$?"

    if [ "${PID}" ]
    then
        if kill -0 "${PID}" >/dev/null 2>/dev/null
        then
            echo "on"
        else
            echo "off"
        fi
    else
        echo "off"
    fi

    return "${RC}"
}


##################################################################################
## NVRAM Functions                                                              ##
##################################################################################

## get NVRAM variable
get_nvram()
{
    VARIABLE="$1"

    /usr/sbin/nvram get "${VARIABLE}" | xargs
    RC="$?"

    return "${RC}"
}

## Save NVRAM variable
set_nvram()
{
    VARIABLE="$1"
    VALUE="$2"

    [ "x$VARIABLE" == "x" ] || [ "x$VALUE" == "x" ] && return 1

    RC="0"

    if [ "$( get_nvram "${VARIABLE}" )" != "${VALUE}" ]
    then
        /usr/sbin/nvram set ${VARIABLE}="${VALUE}"
        RC="$?"
        /usr/sbin/nvram commit
        RC="$?"
    fi

    return "${RC}"
}


##################################################################################
## MQTT Functions                                                               ##
##################################################################################

mqtt_send()
{
    TOPIC="$1"; shift
    MESSAGE="$*"

    OPTIONS="-i ${CAMERA_HOSTNAME}.mqtt_send -h ${MQTT_HOST} -p ${MQTT_PORT}"

    ## If username and password are set: Add them to options
    if [ "x${MQTT_USER}" != "x" ] && [ "x${MQTT_PASS}" != "x" ]
    then
        OPTIONS="${OPTIONS} -u ${MQTT_USER} -P ${MQTT_PASS}"
    fi

    OPTIONS="${OPTIONS} ${MOSQUITTOPUBOPTS} ${MOSQUITTOOPTS}"

    mosquitto_pub ${OPTIONS} -t "${TOPIC}" -m "${MESSAGE}"
}


mqtt_subscribe()
{
    TOPIC="$1"
    ID="$2"

    ## Set the host and port in options
    OPTIONS="-i ${CAMERA_HOSTNAME}.subscribe.${ID} -h $MQTT_HOST -p $MQTT_PORT"

    ## If username and password are set: Add them to options
    if [ "x${MQTT_USER}" != "x" ] && [ "x${MQTT_PASS}" != "x" ]
    then
        OPTIONS="${OPTIONS} -u ${MQTT_USER} -P ${MQTT_PASS}"
    fi

    ## Add generic moquitto options from config.cfg to string
    OPTIONS="${OPTIONS} ${MOSQUITTOOPTS} -v"

    /usr/bin/mosquitto_sub -k 10 ${OPTIONS} -t "${TOPIC}"
}


##################################################################################
##  Status tools                                                                ##
##################################################################################

## Get the system status of the webcam / OS
system_status()
{
    local ARGS="$1"

    UPTIME="$( busybox uptime )"
    SSID="$(    /sbin/iwconfig 2>/dev/null | grep ESSID | sed -e "s/.*ESSID:\"//" | sed -e "s/\".*//" )"
    BITRATE="$( /sbin/iwconfig 2>/dev/null | grep "Bit R" | sed -e "s/   S.*//" | sed -e "s/.*\\://"  )"
    QUALITY="$( /sbin/iwconfig 2>/dev/null | grep "Quali" | sed -e "s/  *//" )"

    NOISE_LEVEL="$(  echo "$QUALITY" | awk '{ print $7}' | sed -e 's/.*=//' | sed -e 's/\/100/\%/' )"
    LINK_QUALITY="$( echo "$QUALITY" | awk '{ print $2}' | sed -e 's/.*=//' | sed -e 's/\/100/\%/' )"
    SIGNAL_LEVEL="$( echo "$QUALITY" | awk '{ print $4}' | sed -e 's/.*=//' | sed -e 's/\/100/\%/' )"

    if [ "$ARGS" == "--json" ]
    then
        echo "{\"uptime\":\"$UPTIME\",\"ssid\":\"$SSID\",\"bitrate\":\"$BITRATE\",\"signal_level\":\"$SIGNAL_LEVEL\",\"link_quality\":\"$LINK_QUALITY\",\"noise_level\":\"$NOISE_LEVEL\"}"
    elif [ "${ARGS}" == "--shell" ]
    then
        echo "UPTIME=\"${UPTIME}\""
        echo "SSID=\"${SSID}\""
        echo "BITRATE=\"${BITRATE}\""
        echo "NOISE_LEVEL=\"${NOISE_LEVEL}\""
        echo "LINK_QUALITY=\"${LINK_QUALITY}\""
        echo "SIGNAL_LEVEL=\"${SIGNAL_LEVEL}\""
    else
        echo "Uptime      : ${UPTIME}"
        echo "SSID        : ${SSID}"
        echo "Bitrate     : ${BITRATE}"
        echo "Noise Level : ${NOISE_LEVEL}"
        echo "Link Quality: ${LINK_QUALITY}"
        echo "Signal Level: ${SIGNAL_LEVEL}"
    fi

    return 0
}

## Get the state of all leds
led_status()
{
    local ARGS="$1"

    BLUE="$(   /tmp/sd/firmware/bin/blue_led -s   | last_f )"
    YELLOW="$( /tmp/sd/firmware/bin/yellow_led -s | last_f )"
    IR_LED="$( /tmp/sd/firmware/bin/ir_led -s     | last_f )"

    if [ "$ARGS" == "--json" ]
    then
        echo "{\"blue_led\":\"$BLUE\",\"yellow_led\":\"$YELLOW\",\"ir_led\":\"$IR_LED\"}"
    elif [ "${ARGS}" == "--shell" ]
    then
        echo "IR_LED=\"$IR_LED\""
        echo "BLUE_LED=\"$BLUE\""
        echo "YELLOW_LED=\"$YELLOW\""
    else
        echo "IR Led    : ${IR_LED}"
        echo "Blue Led  : ${BLUE}"
        echo "Yellow Led: ${YELLOW}"
    fi

    return 0
}

## Get the state of the camera modes
mode_status()
{
    local ARGS="$1"

    NIGHT="$(  /tmp/sd/firmware/bin/nightmode  -s | last_f )"
    IR_CUT="$( /tmp/sd/firmware/bin/ir_cut     -s | last_f )"
    MIRROR="$( /tmp/sd/firmware/bin/mirrormode -s | last_f )"
    FLIP="$(   /tmp/sd/firmware/bin/flipmode   -s | last_f )"

    if [ "$ARGS" == "--json" ]
    then
        echo "{\"mirror_mode\":\"${MIRROR}\",\"flip_mode\":\"${FLIP}\",\"night_mode\":\"$NIGHT\",\"ir_cut\":\"$IR_CUT\"}"

    elif [ "$ARGS" == "--shell" ]
    then
        echo "MIRROR_MODE=\"${MIRROR}\""
        echo "FLIP_MODE=\"${FLIP}\""
        echo "NIGHT_MODE=\"${NIGHT}\""
        echo "IR_CUT=\"${IR_CUT}\""
    else
        echo "Mirror Mode: ${MIRROR}"
        echo "Night Mode : ${NIGHT}"
        echo "Flip Mode  : ${FLIP_MODE}"
        echo "IR Cut     : ${IR_CUT}"
    fi

    return 0
}


##################################################################################
## Create /var/run if nonexistent                                               ##
##################################################################################

if [ ! -d /var/run ]
then
    mkdir -p /var/run
fi

export FUNCTIONS_SOURCED=1

##################################################################################
## EOF                                                                          ##
##################################################################################
