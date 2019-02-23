
# If we're in a bash shell, load the bashrc
if [ "$BASH" ]; then
  if [ -f ~/.bashrc ]; then
    . ~/.bashrc
  fi
fi


# Set the LD library path
export LD_LIBRARY_PATH=/tmp/sd/firmware/lib

# overlay new busybox commands over system ones
. ~/.busybox_aliases

# load functions.sh
. /tmp/sd/firmware/scripts/functions.sh

# Deny no write access to terminal
mesg n || true
