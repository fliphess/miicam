#!/bin/bash
INPUT="$1" ; shift
ARGUMENTS=$*


## Print help output
function usage()
{
    cat <<EOF

    ${0} [--setup-web|--run-web] <arguments>

    A set of utils for devving on the web interface of the MiiCam firmware

    Options:

      --install  - Create required files for running the webui locally

      --run      - Run the php inbuild web server in www/public

    Repo: https://github.com/MiiCam/MiiCamWeb

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

## Symlink config in /tmp/sd to prepare for running the web interface
function setup_web()
{
    echo -ne "*** Creating directories"
    mkdir -p /tmp/sd/log /tmp/sd/firmware/www/public
    echo " [OK]"

    echo -ne "*** Creating config file"
    ../sdcard/firmware/scripts/update/configupdate "/tmp/sd/config.cfg"
    echo " [OK]"

    echo -ne "*** Creating logfiles   "
    echo syslog >> /tmp/sd/log/syslog
    echo webserver >> /tmp/sd/log/lighttpd.log
    echo webapp >>  /tmp/sd/log/webapp.log
    echo bootlog >> /tmp/sd/log/ft_boot.log
    echo motion >> /tmp/sd/log/motion.log
    echo " [OK]"
}


function install()
{
  echo "*** Setting up global composer requirements"
  composer global require hirak/prestissimo mediamonks/composer-vendor-cleaner

  echo "*** Setting up local composer requirements"
  composer install --no-dev --ignore-platform-reqs --no-interaction --prefer-source

  echo "*** Cleaning up"
  php /home/travis/.config/composer/vendor/mediamonks/composer-vendor-cleaner/bin/clean --dir vendor/ >/dev/null
  echo "Done"

  echo "*** Recreating classmap"
  composer dump-autoload --classmap-authoritative

}


## Run the php inbuild webserver in our www directory
function run_web()
{
    log "Starting local php webserver."
    php -S localhost:8080 -t ./public
}

function main()
{
    case "$INPUT"
    in
        --run)
            run_web
        ;;
        --install)
            install
            setup_web
        ;;
        *)
            usage
        ;;
    esac

    exit $?
}

main

