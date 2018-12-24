#################################################################################
##                                                                             ##
##      Development Helper Library                                             ##
##                                                                             ##
#################################################################################

# This is a library with helper functions for repeating tasks while developing
# It can be used to quickly rebuild and upload several daemons.

function renew_make() {
    for file in Makefile OUTPUT.mk SOURCES.mk ; do
        test -f /result/$file && cp -v /result/$file /env/$file
    done
}

function rebuild_binary() {
    local BINARY="$1"
    local MAKEME="/env/build/$BINARY"

    test -f $MAKEME && rm $MAKEME

    cd /env
    renew_make

    echo "Rebuilding $BINARY"
    make $MAKEME
}


function upload_rtsp() {
    cd /env
    grep ROOT_PASS sdcard/config.cfg
    scp prefix/bin/rtspd root@camera3:
}

function rebuild_rtsp() {
    cp -vr /result/tools/rtspd/rtspd.c /result/tools/rtspd/log tools/rtspd/
    rebuild_binary "rtspd"
}


