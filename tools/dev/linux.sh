#!/bin/bash
CACHEDIR="/env/src"

function download() {
    local FILENAME="$1"; shift
    local URL="$1"     ; shift
    local NOCACHE="$1"

    if [[ "$NOCACHE" == "1" -a -f "$FILENAME" ]]; then
        echo "Removing $FILENAME"
        rm "$FILENAME"
    fi

    if [ ! -f "$FILENAME" ] ; then
        wget -O "$FILENAME" "$URL"
    fi
}

[ ! -d "$CACHEDIR" ] && mkdir -p "$CACHEDIR"

echo "*** Downloading linux kernel from toolchain"
download "$CACHEDIR/arm-linux-3.3_2015-01-09.tgz" \
         "https://fliphess.com/toolchain/Software/Embedded_Linux/source/arm-linux-3.3_2015-01-09.tgz" \
         "$NOCACHE"

echo "*** Unpacking linux kernel"
tar xvzf "$CACHEDIR/arm-linux-3.3_2015-01-09.tgz" -C /usr/src

echo "*** Unpacking rootfs"
cd /usr/src/arm-linux-3.3 && tar xvzf target_rootfs.tgz -C .

echo "Unpacked to: /usr/src/arm-linux-3.3"
