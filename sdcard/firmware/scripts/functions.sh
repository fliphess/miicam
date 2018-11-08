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

    if [ "x${SECONDS}" == "x" ]
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

##################################################################################
## Binary manipulation functions                                                ##
##################################################################################

## Creates /tmp/disable-binary
create_disable_binary()
{

    if [ ! -x /tmp/disable-binary ]
    then
        echo -e '#!/bin/sh\necho "$0 disabled with chuangmi-720p-hack"' > /tmp/disable-binary
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

    if ( mount|grep -q "${BINARY}" )
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
##  ISP328 Functions                                                            ##
##################################################################################

## Get ISP328 values
get_isp328()
{
    VARIABLE="$1"

    [ "x$VARIABLE" == "x" ] && return 1

    echo r ${VARIABLE} > /proc/isp328/command

    echo "$( cat /proc/isp328/command )"
    RC="$?"

    return "${RC}"
}

# Set ISP328 values
set_isp328()
{
    VARIABLE="$1"
    VALUE="$2"

    [ "x$VARIABLE" == "x" ] || [ "x$VALUE" == "x" ] &&  return 1

    echo w ${VARIABLE} ${VALUE} > /proc/isp328/command
    RC="$?"

    return "${RC}"
}


##################################################################################
## GPIO Functions                                                               ##
##################################################################################

## Read a value from a GPIO pin
get_gpio()
{
    PIN="$1"
    echo $( cat /sys/class/gpio/gpio${PIN}/value )
    RC="$?"

    return "${RC}"
}

## Write a value to GPIO pin
set_gpio()
{
    PIN="$1"
    VALUE="$2"

    echo "${VALUE}" > /sys/class/gpio/gpio${PIN}/value;
    RC="$?"

    return "${RC}"
}

##################################################################################
## Create Snapshot                                                              ##
##################################################################################

update_snapshot() {
    echo "Creating snapshot to /tmp/sd/firmware/www/public/snapshot.jpg"
    /tmp/sd/firmware/bin/ffmpeg -loglevel fatal -i rtsp://localhost:554/live/ch00_0 -vframes 1 -r 1 -y /tmp/sd/firmware/www/public/snapshot.jpg
}


##################################################################################
## Config manipulation functions                                                ##
##################################################################################

## Set config value in basecfg
set_basecfg()
{
    VARIABLE="$1"
    VALUE="$2"

    if ( egrep -q "^[[:space:]]*${VARIABLE}=(|\")${VALUE}(|\"[[:space:]]*)$" "${BASECFG}" )
    then
        RC="0"
    elif ( grep -q "^[[:space:]]*${VARIABLE}=" "${BASECFG}" )
    then
        sed -i -e "/^[[:space:]]*${VARIABLE}=/ s/=.*/=\"${VALUE}\"/" "${BASECFG}"
        RC="$?"
    else
        echo "${VARIABLE}=\"${VALUE}\"" >> "${BASECFG}"
        RC="$?"
    fi

    return "${RC}"
}


##################################################################################
## LED Control                                                                  ##
##################################################################################

## Control the blue LED
blue_led()
{
    INPUT="$1"

    case "$INPUT" in
        on)
            ${CTRL} LEDSTATUS 0 0 > /dev/null
            RC="$?"
            set_nvram blue_led on
        ;;
        off)
            ${CTRL} LEDSTATUS 0 1 > /dev/null
            RC="$?"
            set_nvram blue_led off
        ;;
        blink)
            ${CTRL} LEDSTATUS 0 2 > /dev/null
            RC="$?"
            set_nvram blue_led blink
        ;;
        status)
            echo $( get_nvram blue_led )
            RC="$?"
        ;;
        *)
            echo "Option $1 not supported"
            RC="1"
        ;;
    esac

    return "${RC}"
}

## Control the yellow LED
yellow_led()
{
    INPUT="$1"

    case "$INPUT" in
        on)
            ${CTRL} LEDSTATUS 1 0 > /dev/null
            RC="$?"
            set_nvram yellow_led on
        ;;
        off)
            ${CTRL} LEDSTATUS 1 1 > /dev/null
            RC="$?"
            set_nvram yellow_led off
        ;;
        blink)
            ${CTRL} LEDSTATUS 1 2 > /dev/null
            RC="$?"
            set_nvram yellow_led blink
        ;;
        status)
            echo "$( get_nvram yellow_led )"
            RC="$?"
        ;;
        *)
            echo "Option $1 not supported"
            RC="1"
        ;;
    esac

    return "${RC}"
}

## Control the infrared LED
ir_led()
{
    INPUT="$1"

    if [ ! -x "${CTRL}" ]
    then
        echo "could not find ${CTRL}"
        return 1
    fi

    case "$INPUT" in
        on)
            ${CTRL} IRLED 255 > /dev/null
            RC="$?"
            set_nvram ir_led on
        ;;
        off)
            ${CTRL} IRLED 0 > /dev/null
            RC="$?"
            set_nvram ir_led off
        ;;
        status)
            echo "$( get_nvram ir_led )"
            RC="$?"
        ;;
        *)
            echo "Option $1 not supported"
            RC="1"
        ;;
    esac

    return "${RC}"
}

## Get the state of all leds
led_status()
{

    YELLOW="$( get_nvram yellow_led )"
    IR_LED="$( get_nvram ir_led     )"
    BLUE="$(   get_nvram blue_led   )"

    echo "{\"blue_led\":\"$BLUE\",\"yellow_led\":\"$YELLOW\",\"ir_led\":\"$IR_LED\"}"

    return 0
}

##################################################################################
## Camera Control                                                              ##
##################################################################################

## Control the infrared filter
ir_cut()
{
    INPUT="$1"

    case "$INPUT" in
        on)
            set_gpio 14 1
            RC="$?"

            set_gpio 15 0
            RC="$?"

            set_nvram ir_cut on
        ;;
        off)
            set_gpio 14 0
            RC="$?"

            set_gpio 15 1
            RC="$?"

            set_nvram ir_cut off
        ;;
        status)
            echo "$( get_nvram ir_cut )"
            RC="$?"
        ;;
        *)
            echo "Option $1 not supported"
            RC="1"
        ;;
    esac

    return "${RC}"
}

## Control the night mode
night_mode()
{
    INPUT="$1"

    case "$INPUT" in
        on)
            if [ "${AUTO_NIGHT_MODE}" -eq 0 ]
            then
                ir_led on
                RC="$?"

                ir_cut off
                RC="$?"

                set_isp328 daynight 1
                RC="$?"
            fi
        ;;
        off)
            if [ "${AUTO_NIGHT_MODE}" -eq 0 ]
            then
                ir_led off
                RC="$?"

                ir_cut on
                RC="$?"

                set_isp328 daynight 0
                RC="$?"
            fi
        ;;
        status)
            STATUS="$( get_isp328 daynight )"
            RC="$?"

            if [ "${AUTO_NIGHT_MODE}" -eq 1 ] || [ "$STATUS" == "DAY_MODE" ]
            then
                echo "off"
            elif [ "$STATUS" == "NIGHT_MODE" ]
            then
                echo "on"
            else
                echo "$STATUS"
            fi
        ;;
        *)
            echo "Option $INPUT not supported"
            RC="1"
        ;;
    esac

    return "${RC}"
}

## Control flip mode
flip()
{
    INPUT="$1"

    case "$INPUT" in
        on)
            set_isp328 flip 1
            RC="$?"
        ;;
        off)
            set_isp328 flip 0
            RC="$?"
        ;;
        status)
            STATUS="$( get_isp328 flip )"
            RC="$?"

            if [ "$STATUS" -eq 0 ]
            then
                echo "off"
            elif [ "$STATUS" -eq 1 ]
            then
                echo "on"
            else
                echo "UNKNOWN"
            fi
        ;;
        *)
            echo "Option $INPUT not supported"
            RC="1"
        ;;
    esac

    return "${RC}"
}

## Control mirror mode
mirror()
{
    INPUT="$1"

    case "$INPUT" in
        on)
            set_isp328 mirror 1
            RC="$?"
        ;;
        off)
            set_isp328 mirror 0
            RC="$?"
        ;;
        status)
            STATUS="$( get_isp328 mirror )"
            RC="$?"

            if [ "$STATUS" -eq 0 ]
            then
                echo "off"
            elif [ "$STATUS" -eq 1 ]
            then
                echo "on"
            else
                echo "UNKNOWN"
            fi
        ;;
        *)
            echo "Option $1 not supported"
            RC="1"
        ;;
    esac

    return "${RC}"
}

##################################################################################
## Image Control                                                                ##
##################################################################################

## Control Brightness
set_brightness()
{
    VALUE="$1"

    [ "$VALUE" -gt 255 ] || [ "$VALUE" -lt 0 ] && die "Use a brightness value between 0 and 255"

    set_isp328 brightness "$VALUE"
    RC="$?"

    return "$RC"
}

get_brightness()
{
    echo $( get_isp328 brightness )
    RC="$?"

    return "$RC"
}

## Set contrast
set_contrast()
{
    VALUE="$1"

    [ "$VALUE" -gt 255 ] || [ "$VALUE" -lt 0 ] && die "Use a contrast value between 0 and 255"

    set_isp328 contrast "$VALUE"
    RC="$?"

    return "$RC"
}

## Get contrast value
get_contrast()
{
    echo $( get_isp328 contrast )
    RC="$?"

    return "$RC"
}

## Set hue
set_hue()
{
    VALUE="$1"

    [ "$VALUE" -gt 255 ] || [ "$VALUE" -lt 0 ] && die "Use a hue value between 0 and 255"

    set_isp328 hue "$VALUE"
    RC="$?"

    return "$RC"
}

## Get hue value
get_hue()
{
    echo $( get_isp328 hue )
    RC="$?"

    return "$RC"
}

## Set saturation
set_saturation()
{
    VALUE="$1"

    [ "$VALUE" -gt 255 ] || [ "$VALUE" -lt 0 ] && die "Use a saturation value between 0 and 255"

    set_isp328 saturation "$VALUE"
    RC="$?"

    return "$RC"
}

## Get saturation value
get_saturation()
{
    echo $( get_isp328 saturation )
    RC="$?"

    return "$RC"
}

## Set denoise
set_denoise()
{
    VALUE="$1"

    [ "$VALUE" -gt 255 ] || [ "$VALUE" -lt 0 ] && die "Use a denoise value between 0 and 255"

    set_isp328 denoise "$VALUE"
    RC="$?"

    return "$RC"
}

## Get denoise value
get_denoise()
{
    echo $( get_isp328 denoise )
    RC="$?"

    return "$RC"
}

## Set sharpness
set_sharpness()
{
    VALUE="$1"

    [ "$VALUE" -gt 255 ] || [ "$VALUE" -lt 0 ] && die "Use a sharpness value between 0 and 255"

    set_isp328 sharpness "$VALUE"
    RC="$?"

    return "$RC"
}

## Get sharpness value
get_sharpness()
{
    echo $( get_isp328 sharpness )
    RC="$?"

    return "$RC"
}

##################################################################################
##  Status tools                                                                ##
##################################################################################

## Get camera status
camera_status()
{
    MIRROR="$( mirror status )"
    FLIP="$( flip status )"
    NIGHT_MODE="$( night_mode status )"
    IR_CUT="$( ir_cut status )"

    BRIGHTNESS="$( get_brightness )"
    CONTRAST="$( get_contrast )"
    HUE="$( get_hue )"
    SATURATION="$( get_saturation )"
    DENOISE="$( get_denoise )"
    SHARPNESS="$( get_sharpness )"

    echo "{\"mirror\":\"$MIRROR\",\"flip\":\"$FLIP\",\"night_mode\":\"$NIGHT_MODE\",\"ir_cut\":\"$IR_CUR\",\"brightness\":\"$BRIGHTNESS\",\"contrast\":\"$CONTRAST\",\"hue\":\"$HUE\",\"denoise\":\"$DENOISE\",\"sharpness\":\"$SHARPNESS\"}"
    RC="$?"

    return "$RC"
}

## Get the system status of the webcam / OS
system_status()
{

    UPTIME="$( busybox uptime )"

    SSID="$(    /sbin/iwconfig 2>/dev/null | grep ESSID | sed -e "s/.*ESSID:\"//" | sed -e "s/\".*//" )"
    BITRATE="$( /sbin/iwconfig 2>/dev/null | grep "Bit R" | sed -e "s/   S.*//" | sed -e "s/.*\\://" )"
    QUALITY="$( /sbin/iwconfig 2>/dev/null | grep "Quali" | sed -e "s/  *//" )"

    NOISE_LEVEL="$(  echo "$QUALITY" | awk '{ print $7}' | sed -e 's/.*=//' | sed -e 's/\/100/\%/' )"
    LINK_QUALITY="$( echo "$QUALITY" | awk '{ print $2}' | sed -e 's/.*=//' | sed -e 's/\/100/\%/' )"
    SIGNAL_LEVEL="$( echo "$QUALITY" | awk '{ print $4}' | sed -e 's/.*=//' | sed -e 's/\/100/\%/' )"

    echo "{\"uptime\":\"$UPTIME\",\"ssid\":\"$SSID\",\"bitrate\":\"$BITRATE\",\"signal_level\":\"$SIGNAL_LEVEL\",\"link_quality\":\"$LINK_QUALITY\",\"noise_level\":\"$NOISE_LEVEL\"}"
    RC="$?"

    return "$RC"
}


##################################################################################
## Set a new http password                                                      ##
##################################################################################

reset_http_password()
{
    echo "root:${ROOT_PASSWORD}" > /tmp/sd/firmware/etc/lighttpd.user
    RC="$?"

    return "$RC"
}


##################################################################################
## Create /var/run if nonexistent                                               ##
##################################################################################

if [ ! -d /var/run ]
then
    mkdir -p /var/run
fi


##################################################################################
## EOF                                                                          ##
##################################################################################
