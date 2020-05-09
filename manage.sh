#!/bin/bash
INPUT="$1" ; shift
ARGUMENTS=$*

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
CONTAINER_IMAGE="miicam/miicam"

CACHEDIR="$HOME/.cache/miicam/src"
test -d "$CACHEDIR" || mkdir -p "$CACHEDIR"

DOCKER_CLI="docker run -i -v ${SCRIPTPATH}:/result -v ${CACHEDIR}:/env/src --detach=false --rm --tty=true"

## Print help output
function usage()
{
    cat <<EOF

    ${0} [--build|--build-docker|--shell|--all] <arguments>

    Manages the container build environment
    to cross compile binaries and build a firmware tarbal

    Options:

      --build          - Runs a container build and then executes make images clean
                         To create a MiiCam.zip and MiiCam.tgz
                         containing the binaries and other contents of the sdcard

      --build-docker   - Only (re)build the container environment

      --shell          - Opens a shell in the container build environment

      --gencert        - Create a self-signed certificate for use with lighttpd

      --release        - Create a new tag and release a new package version

      --run-http-tests - Run the http testsuite to check all web endpoints

      --run-web        - Start a php development server for the local files

      --open-links     - Open all download links in the browser

    Download toolchain: https://fliphess.com/toolchain/
    Repo: https://github.com/MiiCam/MiiCam

EOF

    return 0
}


## Nice output
function log()
{
    MESSAGE="$1"
    STRING=$(printf "%-60s" "*")

    echo "${STRING// /*}"
    echo "*** ${MESSAGE}"
    echo "${STRING// /*}"
}


## Error out
function die()
{
    log "ERROR - $@" > /dev/stderr
    exit 1
}


## Run a command in the container environment
function run()
{
    local COMMAND=$*

    exec $DOCKER_CLI $ARGUMENTS $CONTAINER_IMAGE /bin/bash -c "$COMMAND"

    return $?
}


## Build the container environment
function build_docker()
{
    log "Building docker container environment"

    docker build -t "${CONTAINER_IMAGE}" "${SCRIPTPATH}" $ARGUMENTS

    return $?
}


## Build the firmware image
function build()
{
    log "Building firmware image"

    run 'make images clean && mv /env/MiiCam.zip /env/MiiCam.tgz /result/'

    return $?
}


## Generate a selfsigned certificate
function gencert() {

    if ! ( awk -F/ '$2 == "docker"' /proc/self/cgroup 2>/dev/null | read )
    then
        run '/env/manage.sh --gencert'
    else
        USE_NAME="$( grep ^CAMERA_HOSTNAME sdcard/config.cfg  | cut -d= -f2 | sed -e 's/"//g' )"
        SSLDIR="/result/sdcard/firmware/share/ssl"

        [ -d "${SSLDIR}" ] || mkdir -p "$SSLDIR"

        if [ ! -x "$( command -v openssl )" ]
        then
            die "openssl utility not found."
        fi

        ## Create a root ca key
        if [ ! -f "$SSLDIR/rootCA.key" ]
        then
           echo "Creating a root ca key"
           openssl genrsa -out "$SSLDIR/rootCA.key" 2048
        fi

        ## Create a root ca cert
        if [ ! -f "$SSLDIR/rootCA.pem" ]
        then
            echo "Creating a root ca cert"
            openssl req -x509 -new -nodes -key "$SSLDIR/rootCA.key" -sha256 -days 1024  -out "$SSLDIR/rootCA.pem" || die "Failed to create a root CA Cert"
        fi

        ## Create a config file
        if [ ! -f "$SSLDIR/v3.ext" ]
        then
            echo "Creating certificate config file"
            cat > "$SSLDIR/v3.ext" <<EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names
[alt_names]
DNS.1 = $USE_NAME
DNS.2 = $USE_NAME.local
DNS.3 = $USE_NAME.home
## Add your own aliases here
EOF
            echo "Editting config file"
            vim "$SSLDIR/v3.ext"
        fi

        if [ ! -f "$SSLDIR/server.csr" ]
        then
            ## Create a CSR and a key at once
            openssl req -new -nodes -out "$SSLDIR/server.csr" -newkey rsa:2048 -keyout "$SSLDIR/server.key"
        fi

        if [ ! -f "$SSLDIR/server.crt" ]
        then
            ## Create a certificate
            openssl x509 \
                -req \
                -in "$SSLDIR/server.csr" \
                -CA "$SSLDIR/rootCA.pem" \
                -CAkey "$SSLDIR/rootCA.key" \
                -CAcreateserial \
                -out "$SSLDIR/server.crt" \
                -days 500 \
                -sha256 \
                -extfile "$SSLDIR/v3.ext"
        fi

        if [ ! -f "$SSLDIR/server.pem" ]
        then
            ## Combine files into a single PEM file
            cat "$SSLDIR/server.key" "$SSLDIR/server.crt" > "$SSLDIR/server.pem"
        fi

        if [ ! -f "$SSLDIR/dh2048.pem" ]
        then
            echo "Generating DH Params file"
            openssl dhparam -out "$SSLDIR/dh2048.pem" -outform PEM -2 2048
        fi

        echo "The certificates are created in $SSLDIR. You can load rootCA.pem in your browser to trust the connection"
    fi
}

function open_links()
{
    [ -f "$SCRIPTPATH/sources.json" ] || die "Sources file $SCRIPTPATH/sources.json not found!"

    local LINKS="$( cat "$SCRIPTPATH/sources.json" | jq -r 'values[].website' )"

    echo "Opening links in browser....."
    for link in ${LINKS}; do
        open "$link"
    done
}

## Release a new version
function release()
{
   [ -x "$( command -v gitsem )" ] || die "This script depends on gitsem: go get github.com/Clever/gitsem"
   [ "$( git rev-parse --abbrev-ref HEAD )" == "master" ] || die "You are not on the master branch"
   [ "$( git diff --stat )" == '' ] || die "Git repo is dirrrrrrrty"

   echo "Releasing $VERSION"
   gitsem newversion
   echo "Don't forget to push your tags :)"
}

## Symlink config in /tmp/sd to prepare for running the web interface
function setup_web()
{
    if [ "${NO_SETUP}" == "1" ]
    then
	return 0
    fi

    echo "*** Setting up global composer requirements"
    composer global require hirak/prestissimo mediamonks/composer-vendor-cleaner

    echo "*** Creating directories"
    mkdir -p /tmp/sd/log /tmp/sd/firmware/www/public

    echo "*** Creating config file"
    sdcard/firmware/scripts/configupdate "/tmp/sd/config.cfg"

    echo "*** Creating logfiles"
    echo syslog >> /tmp/sd/log/syslog
    echo webserver >> /tmp/sd/log/lighttpd.log
    echo webapp >>  /tmp/sd/log/webapp.log
    echo bootlog >> /tmp/sd/log/ft_boot.log
    echo motion >> /tmp/sd/log/motion.log
    echo "rtspd.log" >> /tmp/sd/log/rtspd.log

    echo "*** Setting up local composer requirements"
    cd web
    composer install --no-dev --ignore-platform-reqs --no-interaction --prefer-source

    echo "*** Cleaning up"
    php $HOME/.config/composer/vendor/mediamonks/composer-vendor-cleaner/bin/clean --dir vendor/ >/dev/null

    echo "*** Recreating classmap"
    composer dump-autoload --classmap-authoritative
}


## Run the php inbuild webserver in our www directory
function run_web()
{
    log "*** Starting local php webserver."
    cd "${SCRIPTPATH}"
    php -S localhost:8080 -t web/public
}


## Run http testsuite
function run_http_tests()
{
    if ! ( awk -F/ '$2 == "docker"' /proc/self/cgroup 2>/dev/null | read )
    then
        run '/env/manage.sh --run-http-tests'
    else
        cd /env/
        export CAMERA_HOSTNAME="$( cat /env/tools/dev/host.cfg )"
        nosetests -v tests/http/
    fi
}

## Spawn a shell in the container environment
function shell()
{
    log "Opening a bash shell in the container environment"

    run /bin/bash

    return $?
}


function main()
{
    case "$INPUT"
    in
        --build)
            build
        ;;
        --build-docker)
            build_docker
        ;;
        --shell)
            shell
        ;;
        --newshell)
            build_docker
            shell
        ;;
        --release)
            release
        ;;
        --open-links)
            open_links
        ;;
        --gencert)
            gencert
        ;;
        --run-http-tests)
            run_http_tests
        ;;
        --run-web)
	    setup_web && run_web
	;;
        --all)
            build_docker
            build
        ;;
        *)
            usage
        ;;
    esac

    exit $?
}

main

