#################################################################################
##                                                                             ##
##      Development Helper Library                                             ##
##                                                                             ##
#################################################################################

# This is a library with helper functions for repeating tasks while developing
# It can be used to quickly rebuild and upload several daemons.

if [ -f tools/dev/host.cfg ]
then
    export CAMERA_HOSTNAME="$( cat tools/dev/host.cfg )"
fi

export TOOLCHAINDIR="/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin"
export PATH="${TOOLCHAINDIR}:${PATH}:$HOME/.composer/vendor/mediamonks/composer-vendor-cleaner/bin"

export TARGET="arm-unknown-linux-uclibcgnueabi"
export AR="${TARGET}-ar"
export AS="${TARGET}-as"
export CC="${TARGET}-gcc"
export CXX="${TARGET}-g++"
export LD="${TARGET}-ld"
export NM="${TARGET}-nm"
export RANLIB="${TARGET}-ranlib"
export STRIP="${TARGET}-strip"

export TOPDIR="/env"
export SOURCEDIR="${TOPDIR}/src"
export PREFIXDIR="${TOPDIR}/prefix"
export BUILDDIR="${TOPDIR}/build"
export INSTALLDIR="${TOPDIR}/sdcard/firmware/bin"
export WEBROOT="${TOPDIR}/sdcard/firmware/www"


function log() {
    MESSAGE="$1"
    STRING=$(printf "%-60s" "*")

    echo "${STRING// /*}"
    echo "*** ${MESSAGE}"
    echo "${STRING// /*}"
}


function get_password() {
    log $( grep ROOT_PASS sdcard/firmware/scripts/update/configupdate )
}

###############################################
## Copy makefiles from /result               ##
###############################################

function renew_make() {
    for file in Makefile tools/make/*.mk ; do
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

alias rb="rebuild_binary"

function upload_binary() {
    local FILENAME="$1"
    cd /env/

    if [ ! -f "$FILENAME" ] ; then
        echo "Filename not found: $FILENAME."
        return 1
    fi

    get_password
    scp "$FILENAME" root@$CAMERA_HOSTNAME:/tmp/sd/firmware/bin
}


function build() {
    tool="$1"; shift
    cd /env/tools/gm_lib

    arm-unknown-linux-uclibcgnueabi-gcc \
        -Wall                           \
        -I /env/tools/gm_lib/inc        \
        -I /env/tools/lib               \
        -I /env/prefix/include          \
        -L /env/tools/lib               \
        -L /env/tools/gm_lib/lib        \
        -L /env/prefix/lib              \
        -o /env/tools/bin/${tool}       \
        /env/tools/gm_lib/${tool}.c     \
        -l gm                           \
        -l m                            \
        -l popt                         \
        -l rt                           \
        -l pthread

    arm-unknown-linux-uclibcgnueabi-strip /env/tools/bin/${tool}
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


