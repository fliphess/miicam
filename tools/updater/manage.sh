#!/bin/bash
SCRIPT="$0"
INPUT="$1" ; shift
ARGUMENTS=$*

export COMPOSER_HOME="$HOME/.config/composer"

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

## Print help output
function usage()
{
    cat <<!

    ${0} [--setup-web|--run-web|--build] <arguments>

    A set of utils for devving on the updater utility for the MiiCam firmware

    Options:

      --build    - Build the package

      --release  - Release the package

    Repo: https://github.com/MiiCam/MiiCamWeb

!

    return 0
}

build_package()
{
    cd "$( dirname ${SCRIPT} )"
    log "Installing MiiCamUpdater"
    pip3 install --upgrade .
}


function release()
{
   [ "$( git rev-parse --abbrev-ref HEAD )" == "master" ] || die "You are not on the master branch"
   [ "$( git diff --stat )" == '' ] || die "Git repo is dirrrrrrrty"
   VERSION="$(date +'%Y%m%d%H%M%S')"
   echo "Releasing $VERSION"
   git tag $VERSION
   echo "Don't forget to push your tags :)"
}


function main()
{
    case "$INPUT"
    in
        --build)
            build_package
        ;;
        --release)
            release
        ;;
        *)
            usage
        ;;
    esac

    exit $?
}

main

