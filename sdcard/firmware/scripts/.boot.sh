#!/bin/sh
##################################################################################
## purpose: Start enabled services and stop with LED blinking                   ##
## license: GPLv3+, http://www.gnu.org/licenses/gpl-3.0.html                    ##
## author: Jan Sperling , 2017                                                  ##
##################################################################################

SD_MOUNTDIR="/tmp/sd"

if [ -r "${SD_MOUNTDIR}/firmware/scripts/functions.sh" ]
then
    . "${SD_MOUNTDIR}/firmware/scripts/functions.sh"
else
    echo "Unable to load basic functions"
    exit 1
fi

export LOGFILE="${LOGDIR}/ft_boot.log"
export LD_LIBRARY_PATH=/tmp/sd/firmware/lib

(

echo "*** Executing /mnt/data/test/boot.sh... "

##################################################################################
## Put our bins into PATH                                                       ##
##################################################################################

if [ -d "${SD_MOUNTDIR}/firmware/bin" ] && ! mountpoint -q /tmp/sd/ft
then
    echo "*** Mounting ${SD_MOUNTDIR}/firmware/bin on /tmp/sd/ft... "
    mount --rbind "${SD_MOUNTDIR}/firmware/bin" /tmp/sd/ft
fi


##################################################################################
## Wait for network with booting                                                ##
##################################################################################

if [ "$WAIT_FOR_NETWORK" -eq 1 ]
then
    wait_for_network_until "$PING_RETRIES" "$PING_WAIT" "$PING_IP"
fi

##################################################################################
## Start enabled services                                                       ##
##################################################################################

####################################
## Status LED                     ##
####################################

## Blue on
/tmp/sd/firmware/bin/blue_led -e

## Disable the others (will be changed using restore state if required)
${SD_MOUNTDIR}/firmware/bin/yellow_led -d
${SD_MOUNTDIR}/firmware/bin/ir_led -d
${SD_MOUNTDIR}/firmware/bin/ir_cut -d

####################################
## Restore settings               ##
####################################

if [ "${RESTORE_STATE}" -eq 1 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99restore_state start
fi

####################################
## Telnetd                        ##
####################################

if [ "${ENABLE_TELNETD}" -eq 1 ] || [ "${ENABLE_SSHD}"  -eq 0 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99telnet start

    if ! grep -q '^telnetd' /tmp/etc/restartd.conf
    then
        echo "telnetd \"/usr/sbin/telnetd\" \"${SD_MOUNTDIR}/firmware/etc/init/S99telnet restart\" \"/bin/echo '*** telnetd was restarted from restartd... '\"" >> /tmp/etc/restartd.conf
    fi
else
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99telnet stop
fi

####################################
## Dropbear SSH                   ##
####################################

if [ "${ENABLE_SSHD}" -eq 1 ] || [ "${ENABLE_TELNETD}" -eq 0 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99dropbear start

    if ! grep -q '^dropbear' /tmp/etc/restartd.conf
    then
        echo "dropbear \"${SD_MOUNTDIR}/firmware/bin/dropbear\" \"${SD_MOUNTDIR}/firmware/etc/init/S99dropbear restart\" \"/bin/echo '*** Dropbear SSH was restarted from restartd... '\"" >> /tmp/etc/restartd.conf
    fi
fi

####################################
## Crond                          ##
####################################

if [ "${ENABLE_CRON}" -eq 1 ]
then
    ## Load the crontab file by restarting the daemon
    if [ -s "${SD_MOUNTDIR}/firmware/etc/crontab" ]
    then
        sh ${SD_MOUNTDIR}/firmware/etc/init/S99crond restart
    fi

    ## Setup restartd
    if ! grep -q '^crond' /tmp/etc/restartd.conf
    then
        echo "crond \"/usr/sbin/crond\" \"${SD_MOUNTDIR}/firmware/etc/init/S99crond restart\" \"/bin/echo '*** crond was restarted from restartd... '\"" >> /tmp/etc/restartd.conf
    fi
else
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99crond stop
fi

####################################
## NTPd                           ##
####################################

sh ${SD_MOUNTDIR}/firmware/etc/init/S51ntpd start

if ! grep -q '^ntpd' /tmp/etc/restartd.conf
then
    echo "ntpd \"/usr/sbin/ntpd\" \"${SD_MOUNTDIR}/firmware/etc/init/S51ntpd restart\" \"/bin/echo '*** NTPd was restarted from restartd... '\"" >> /tmp/etc/restartd.conf
fi

####################################
## Lighttpd Webserver             ##
####################################

if [ "${ENABLE_HTTPD}" -eq 1 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99lighttpd start
fi

####################################
## FTP server                     ##
####################################

if [ "${ENABLE_FTPD}" -eq 1 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99ftpd start
fi

####################################
## RTSP server                    ##
####################################

if [ "${ENABLE_RTSP}" -eq 1 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99rtsp start
fi

####################################
## Auto Night Mode                ##
####################################

if [ "${AUTO_NIGHT_MODE}" -eq 1 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99auto_night_mode start
else
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99auto_night_mode stop

fi

####################################
## MQTT                           ##
####################################

if [ "${ENABLE_MQTT}" -eq 1 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99mqtt-interval start
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99mqtt-control  start
fi

####################################
## RestartD                       ##
####################################

if [ "$ENABLE_RESTARTD" -eq 1 ]
then
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99restartd restart
else
    sh ${SD_MOUNTDIR}/firmware/etc/init/S99restartd stop
fi

####################################
## Ceiling camera mode            ##
####################################

if [ "$CEILING_MODE" -eq 1 ]
then
    /tmp/sd/firmware/bin/flipmode -e
    /tmp/sd/firmware/bin/mirrormode -e
fi

##################################################################################
## Cleanup                                                                      ##
##################################################################################

if [ -f /mnt/data/test/boot.sh ]
then
    rm /mnt/data/test/boot.sh
fi


) >> "${LOGFILE}" 2>&1

