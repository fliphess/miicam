#!/bin/sh
FUNCTION="$1" ; shift
ARGUMENTS="$*"

SD_MOUNTDIR="/tmp/sd"
if [ -r "${SD_MOUNTDIR}/firmware/scripts/functions.sh" ]
then
    . "${SD_MOUNTDIR}/firmware/scripts/functions.sh"
else
    echo "Unable to load basic functions"
    exit 1
fi


if [ "x$FUNCTION" == "x" ]
then
    cat <<EOF

    $0 <function> <arguments>

    Run a function from the function library
    Available functions are:

$( grep '()' ${SD_MOUNTDIR}/firmware/scripts/functions.sh | tr -d '()' | xargs printf "      - %s\n"  )

EOF
fi

$FUNCTION $ARGUMENTS
