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
            openssl x509 -req -in "$SSLDIR/server.csr" -CA "$SSLDIR/rootCA.pem" -CAkey "$SSLDIR/rootCA.key" -CAcreateserial -out "$SSLDIR/server.crt" -days 500 -sha256 -extfile "$SSLDIR/v3.ext"
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

