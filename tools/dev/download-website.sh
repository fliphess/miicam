#!/bin/sh

WEBSITE_URL="$( curl -s https://api.github.com/repos/miicam/miicamweb/releases/latest | grep browser_download_url | grep tgz | awk '{print $NF}' | tr -d '"' )"

if [ "x$WEBSITE_URL" == "x" ]
then
    echo "Website url not found! Exiting build"
    exit 1
fi

test -d src || mkdir -p src

wget -t 5 -T 10 -c -O src/website.tgz "$WEBSITE_URL"
