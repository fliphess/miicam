#!/bin/sh

##################################################################################
## purpose: Initialize the Chuangmi 720P hack                                   ##
## license: GPLv3+, http://www.gnu.org/licenses/gpl-3.0.html                    ##
## author: Jan Sperling , 2017                                                  ##
##################################################################################

if [ ! -d "${LOGDIR}" ]
then
    mkdir -p "${LOGDIR}"
fi

##################################################################################
## Config                                                                       ##
##################################################################################

export SD_MOUNTDIR="/tmp/sd"

if [ -f "${SD_MOUNTDIR}/config.cfg" ]
then
    . "${SD_MOUNTDIR}/config.cfg"
else
    echo "Config not found! Running normal boot!" | tee -a "${LOGFILE}"
    echo 0 > /tmp/ft_mode
    do_vg_boot
    exit
fi

if [ "${DISABLE_HACK}" -eq 1 ]
then
    echo "Hack disabled, proceed with default start" | tee -a "${LOGFILE}"
    echo 0 > /tmp/ft_mode
    do_vg_boot
    exit
fi

if [ "${PURGE_LOGFILES_AT_BOOT}" -eq 1 ]
then
    if [ -d "${SD_MOUNTDIR}/log" ]
    then
        echo "*** Purging old log files... "
        rm ${SD_MOUNTDIR}/log/*
    fi
fi

(
cat << EOF

Running Chuangmi 720P hack

Chuangmi 720P hack configuration:

  HOSTNAME       = ${HOSTNAME}
  WIFI_SSID      = ${WIFI_SSID}
  WIFI_PASS      = ${WIFI_PASS}
  TIMEZONE       = ${TIMEZONE}
  NTP_SERVER     = ${NTP_SERVER}
  DISABLE_CLOUD  = ${DISABLE_CLOUD}
  DISABLE_OTA    = ${DISABLE_OTA}
  ENABLE_TELNETD = ${ENABLE_TELNETD}
  ENABLE_SSHD    = ${ENABLE_SSHD}
  ENABLE_HTTPD   = ${ENABLE_HTTPD}
  ENABLE_FTPD    = ${ENABLE_FTPD}
  ENABLE_SAMBA   = ${ENABLE_SAMBA}
  ENABLE_RTSP    = ${ENABLE_RTSP}

EOF


##################################################################################
## Syslog                                                                       ##
##################################################################################


sh "${SD_MOUNTDIR}/firmware/etc/init/S01logging" restart


##################################################################################
## Make /etc writeable                                                          ##
##################################################################################

echo "*** Making /etc writable"

if ! [ -d /tmp/etc ]
then
    cp -r /etc /tmp/
    cp -r ${SD_MOUNTDIR}/firmware/etc/* /tmp/etc
fi

if ! mountpoint -q /etc
then
    mount --rbind /tmp/etc /etc
fi

##################################################################################
## Mount /root from sd card                                                     ##
##################################################################################

echo "*** Mounting /root from sd card"

if ! [ -d /tmp/root ]
then
    cp -r ${SD_MOUNTDIR}/firmware/root /tmp/root
    chown -R root:root /tmp/root
    chmod -R 0750 /tmp/root
fi

if ! mountpoint -q /root
then
    mount --rbind /tmp/root /root
fi

##################################################################################
## Prepare restartd.conf                                                        ##
##################################################################################

echo "*** Replacing restartd startup script and config with our own version"
if ! mount | grep -q /mnt/data/imi/imi_init/S99restartd
then
    mount --bind /etc/init/S99restartd /mnt/data/imi/imi_init/S99restartd
fi

if [ ! -f /tmp/etc/restartd.conf.org ] && mountpoint -q /etc
then
    cp /mnt/data/restartd/restartd.conf /tmp/etc/restartd.conf.org
    cp /mnt/data/restartd/restartd.conf /tmp/etc/restartd.conf
fi


##################################################################################
## Set time zone                                                                ##
##################################################################################

if [ -n "${TIMEZONE}" ]
then
    echo "*** Configure time zone... "
    rm /tmp/etc/TZ
    echo "${TIMEZONE}" > /tmp/etc/TZ
    export TZ="${TIMEZONE}"
fi


##################################################################################
## Set hostname and format /etc/hosts                                           ##
##################################################################################

if [ -n "${HOSTNAME}" ]
then
    echo "*** Setting hostname... "
    echo "${HOSTNAME}" > /etc/hostname
    hostname "${HOSTNAME}"

    echo "*** Configuring new /etc/hosts file... "
    echo -e "127.0.0.1 \tlocalhost\n127.0.1.1 \t$HOSTNAME\n\n" > /etc/hosts

    if [ -f "${SD_MOUNTDIR}/firmware/etc/hosts" ]
    then
        echo "*** Appending ${SD_MOUNTDIR}/firmware/etc/hosts to /etc/hosts"
        cat ${SD_MOUNTDIR}/firmware/etc/hosts >> /etc/hosts
    fi
fi


##################################################################################
## Set root Password                                                            ##
##################################################################################

if [ -n "${ROOT_PASSWORD}" ]
then
    echo "*** Setting root password... "
    echo "root:${ROOT_PASSWORD}" | chpasswd

    if [ -f "${SD_MOUNTDIR}/firmware/bin/smbpasswd" ]
    then
        if ! [ -d "${SD_MOUNTDIR}/firmware/tmp/samba" ]
        then
            mkdir -p "${SD_MOUNTDIR}/firmware/tmp/samba"
        fi

        printf "*** Setting Samba root password... "
        (echo "${ROOT_PASS}"; echo "${ROOT_PASS}") | "${SD_MOUNTDIR}/firmware/bin/smbpasswd" -a -s 2>&1
    fi
else
    echo "WARN: root password must be set for SSH and SAMBA"
fi


##################################################################################
## WIFI                                                                         ##
##################################################################################

if [ -s "${SD_MOUNTDIR}/firmware/scripts/configure_wifi" ]
then
    echo "*** Configuring WIFI... "
    sh "${SD_MOUNTDIR}/firmware/scripts/configure_wifi"
fi

##################################################################################
## Disable Cloud Services and OTA                                               ##
##################################################################################

if [ "${DISABLE_CLOUD}" -eq 1 ]
then
    sh "${SD_MOUNTDIR}/firmware/etc/init/S50disable_cloud" start
    sh "${SD_MOUNTDIR}/firmware/etc/init/S50disable_ota" start

elif [ "${DISABLE_OTA}" -eq 1 ]
then
    sh "${SD_MOUNTDIR}/firmware/etc/init/S50disable_ota" start
else
    sh "${SD_MOUNTDIR}/firmware/etc/init/S50disable_ota" stop
fi


##################################################################################
## Start enabled Services                                                       ##
##################################################################################

if ! [ -f /mnt/data/test/boot.sh ]
then
    ln -s -f ${SD_MOUNTDIR}/firmware/scripts/.boot.sh /mnt/data/test/boot.sh
fi

) >> "${LOGFILE}" 2>&1
