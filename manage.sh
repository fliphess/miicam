#!/bin/bash
INPUT="$1" ; shift
ARGUMENTS=$*

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
CONTAINER_IMAGE="chuangmi-720p-hack"
DOCKER_CLI="docker run -i -v ${SCRIPTPATH}:/result --detach=false --tty=true"

## Print help output
function usage()
{
    cat <<EOF

    ${0} [--build|--build-docker|--shell|--all] <arguments>

    Manages the container build environment
    to cross compile binaries and build a firmware tarbal

    Options:

      --build        - Runs a container build and then executes make images clean
                       To create a chuangmi-720p-hack.zip and chuangmi-720p-hack.tgz
                       containing the binaries and other contents of the sdcard

      --build-docker - Only (re)build the container environment

      --shell        - Opens a shell in the container build environment

      --setup-web    - Create required files for running the webui locally

      --run-web      - Run the php inbuild web server in www/public

      --gencert      - Create a self-signed certificate for use with lighttpd

    Download toolchain: https://fliphess.com/toolchain/
    Repo: https://github.com/fliphess/chuangmi-720p-hack

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

    run 'make images clean && mv /env/chuangmi-720p-hack.zip /env/chuangmi-720p-hack.tgz /result/'

    return $?
}

## Symlink config in /tmp/sd to prepare for running the web interface
function setup_web() {
    echo -ne "*** Creating directories"
    mkdir -p /tmp/sd/log /tmp/sd/firmware/www/public
    echo " [OK]"

    echo -ne "*** Creating config file"
    ln -sf "$(pwd)/sdcard/config.cfg"  "/tmp/sd/config.cfg"
    echo " [OK]"

    echo -ne "*** Creating logfiles   "
    echo syslog >> /tmp/sd/log/syslog
    echo webserver >> /tmp/sd/log/lighttpd.log
    echo webapp >>  /tmp/sd/log/webapp.log
    echo bootlog >> /tmp/sd/log/ft_boot.log
    echo motion >> /tmp/sd/log/motion.log
    echo " [OK]"
}


## Run the php inbuild webserver in our www directory
function run_web() {
    log "Starting local php webserver."
    cd sdcard/firmware/www
    php -S localhost:8080 -t ./public
}

## Generate a selfsigned certificate
function gencert() {
    source sdcard/config.cfg

    USE_NAME="$( grep ^HOSTNAME sdcard/config.cfg  | cut -d= -f2 | sed -e 's/"//g' )"

    SSLDIR="sdcard/firmware/etc/ssl"
    [ -d "${SSLDIR}" ] || mkdir -p "$SSLDIR"

    if [ ! -x "$( command -v openssl )" ]
    then
        echo "openssl utility not found."
        exit 1
    fi

   ## Create a root ca key
   echo "Creating a root ca key"
   openssl genrsa -out "$SSLDIR/rootCA.key" 2048 || exit 1

   ## Create a root ca cert
   echo "Creating a root ca cert"
   openssl req -x509 -new -nodes -key "$SSLDIR/rootCA.key" -sha256 -days 1024  -out "$SSLDIR/rootCA.pem" || exit 1

   ## Create a config file
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

    ## Create a CSR and a key at once
    openssl req -new -nodes -out "$SSLDIR/server.csr" -newkey rsa:2048 -keyout "$SSLDIR/server.key"

    ## Create a certificate
    openssl x509 -req -in "$SSLDIR/server.csr" -CA "$SSLDIR/rootCA.pem" -CAkey "$SSLDIR/rootCA.key" -CAcreateserial -out "$SSLDIR/server.crt" -days 500 -sha256 -extfile "$SSLDIR/v3.ext"

    echo "The certificates are created in $SSLDIR. You can load rootCA.pem in your browser to trust the connection"
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
        --setup-web)
            setup_web
        ;;
        --run-web)
            run_web
        ;;
        --gencert)
            gencert
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

