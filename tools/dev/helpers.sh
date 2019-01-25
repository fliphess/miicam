#################################################################################
##                                                                             ##
##      Development Helper Library                                             ##
##                                                                             ##
#################################################################################

# This is a library with helper functions for repeating tasks while developing
# It can be used to quickly rebuild and upload several daemons.

export CAMERA_HOSTNAME="camera3"


function log() {
    MESSAGE="$1"
    STRING=$(printf "%-60s" "*")

    echo "${STRING// /*}"
    echo "*** ${MESSAGE}"
    echo "${STRING// /*}"
}


function get_password() {
    log $( grep ROOT_PASS sdcard/config.cfg )
}

###############################################
## Copy makefiles from /result               ##
###############################################

function renew_make() {
    for file in Makefile OUTPUT.mk SOURCES.mk ; do
        test -f /result/$file && cp -v /result/$file /env/$file
    done
}


###############################################
## Build tools                               ##
###############################################

function rebuild_binary() {
    local BINARY="$1"
    local MAKEME="/env/build/$BINARY"

    test -f $MAKEME && rm $MAKEME

    cd /env
    renew_make

    echo "Rebuilding $BINARY"
    make $MAKEME
}

function upload_binary() {
    local FILENAME="$1"
    cd /env/

    if [ ! -f "$FILENAME" ] ; then
        echo "Filename not found: $FILENAME."
        return 1
    fi

    get_password
    scp "$FILENAME" root@$CAMERA_HOSTNAME:
}


###############################################
## Utils and libs helpers                    ##
###############################################

function rebuild_libs() {
    cd /env/
    rm build/chuangmi_*
    cp -vr /result/tools/lib/* tools/lib/
    make libs
}

function upload_libs() {
    cd /env/
    get_password
    scp tools/lib/*.so root@$CAMERA_HOSTNAME:/tmp/sd/firmware/lib/
}

function rebuild_utils() {
    cd /env/
    rm build/bin/*
    cp -vr /result/tools/utils/* tools/utils/
    make utils
}

function upload_utils() {
    cd /env/
    get_password
    scp tools/bin/* root@$CAMERA_HOSTNAME:/tmp/sd/firmware/bin/
}

###############################################
## RTSP Helpers                              ##
###############################################

function upload_rtsp() {
    cd /env
    get_password
    scp tools/bin/rtspd root@$CAMERA_HOSTNAME:/tmp/sd/firmware/bin/
}

function rebuild_rtsp() {
    cp -vr /result/tools/rtspd/rtspd.c /result/tools/rtspd/log tools/rtspd/
    rebuild_binary "rtspd"
}


