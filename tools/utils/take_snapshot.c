#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "chuangmi_utils.h"


int main(int argc, char *argv[])
{
    if (request_snapshot() < 0) {
        fprintf(stderr, "*** Error: Failed to request a snapshot by writing to %s\n", RTSPD_REQUEST_SNAPSHOT);
        return EXIT_FAILURE;
    }

    // * Wait until file is removed
    if (wait_for_file_removal(RTSPD_REQUEST_SNAPSHOT) == 0) {
        fprintf(stderr, "*** Success: Snapshot created!\n");
    }
    else {
        fprintf(stderr, "*** Error: No snapshot created! Is rtspd running?\n");
        return EXIT_FAILURE;
    }

    // * Retrieve filename of last created snapshot
    int ret = get_last_snapshot();

    return ret;
}

