#!/bin/sh
WEBSITE_URL=""

RETRIES=10
SLEEP=1

TRIES=0
URLS_FOUND=0

##################################################
## Retry until urls are retrieved               ##
##################################################

until [ "$TRIES" -ge "$RETRIES" ] || [ "$URLS_FOUND" -eq 2 ]
do
    URLS_FOUND=0
    WEBSITE_URLS="$( curl -s https://api.github.com/repos/miicam/miicamweb/releases/latest | jq -r '.assets | to_entries[] | .value.browser_download_url' )"
    for URL in $WEBSITE_URLS
    do
        if ( echo "$URL" | grep -qE "https://github.com/miicam/MiiCamWeb/releases/download/([0-9]+)/website." )
        then
            echo "Found: $URL"
            URLS_FOUND="$(( $URLS_FOUND +1 ))"
        fi
    done

    TRIES="$(( $TRIES +1 ))"
    sleep "$SLEEP"
done


##################################################
## Check if urls are found and download         ##
##################################################

if [ "$TRIES" -ge "$RETRIES" ] || [ "x$WEBSITE_URLS" = "x" ]
then
   echo " FAIL"
   echo "*** No download urls not found! Sad panda :("
   exit 1
else
    test -d src || mkdir -p src

    cd src

    for URL in $WEBSITE_URLS
    do
        echo "Getting $URL ..."
        wget -q -t 5 -T 10 -c "$URL"
        if [ "$?" != 0 ]
        then
            echo "Error downloading $URL"
            exit 1
        fi
    done
fi


##################################################
## Check md5sum                                 ##
##################################################

if [ ! -f  "website.md5sum" ]
then
    echo "ERROR: website.md5sum not found"
    exit 1
fi

if md5sum --status --check "website.md5sum"
then
    echo "Sums OK: $( cat website.md5sum  )"
    exit 0
else
    echo "An error appeared during download: The sums don't match!"
    echo "Sum in file: $( cat website.md5sum )"
    echo "Actual Sum: $( md5sum website.md5sum )"
    exit 1
fi

