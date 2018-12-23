
# Set a nice prompt
export PS1='\[\033[01;32m\]\u@\h \[\033[01;34m\]\W\[\033[01;31m\]\[\033[01;32m\] \$ \[\033[00m\]'

# Set the LD library path
export LD_LIBRARY_PATH=/tmp/sd/firmware/lib

# Set the path
export PATH="/tmp/sd/firmware/bin:/tmp/sd/firmware/scripts:$PATH"

# Set the correct terminal
export TERM=xterm-color

# overlay new busybox commands over system ones
. ~/.busybox_aliases

# load functions.sh
. /tmp/sd/firmware/scripts/functions.sh
