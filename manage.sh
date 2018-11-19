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

    run 'make images clean && mv /build/chuangmi-720p-hack.zip /build/chuangmi-720p-hack.tgz /result/'

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
    echo " [OK]"
}


## Run the php inbuild webserver in our www directory
function run_web() {
    log "Starting local php webserver."
    cd sdcard/firmware/www
    php -S localhost:8080 -t ./public
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

