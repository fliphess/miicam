#!/bin/sh
WEBSITE_URL=""

RETRIES=10
SLEEP=1

TRIES=0
until [ "$TRIES" -ge "$RETRIES" ]
do
    WEBSITE_URL="$( curl -s https://api.github.com/repos/miicam/miicamweb/releases/latest | jq -r '.assets | to_entries[] | .value.browser_download_url' )"

    if ( echo "$WEBSITE_URL" | grep -qE "https://github.com/miicam/MiiCamWeb/releases/download/([0-9]+)/website.tgz" )
    then
        break
    fi

    TRIES=$(( $TRIES+1 ))
    sleep "$SLEEP"
done

if [ "$TRIES" -ge "$RETRIES" ] || [ "x$WEBSITE_URL" = "x" ]
then
   echo " FAIL"
   echo "*** No download url found! Sad panda :("
   exit 1
else
    test -d src || mkdir -p src

    echo "Getting ${WEBSITE_URL} ..."
    wget -q -t 5 -T 10 -c -O src/website.tgz "$WEBSITE_URL"
    exit $?
fi

