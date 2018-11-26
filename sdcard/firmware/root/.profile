
# Set LDD Library PATH
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/tmp/sd/firware/lib";

# Set the path
PATH="/tmp/sd/firmware/bin:/tmp/sd/firmware/scripts:$PATH"

# Set the correct terminal
TERM=xterm-color

# overlay new busybox commands over system ones
. ~/.busybox_aliases

# load functions.sh
. /tmp/sd/firmware/scripts/functions.sh
