#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chuangmi_utils.h"


int main(int argc, char *argv[])
{
    if (request_video() < 0) {
        fprintf(stderr, "*** Error: Failed to request a video by writing to %s\n", RTSPD_REQUEST_VIDEO);
        return EXIT_FAILURE;
    }

    if (wait_for_file_removal(RTSPD_REQUEST_VIDEO) == 0) {
        fprintf(stderr, "*** Success: Video created!\n");
    }
    else {
        fprintf(stderr, "*** Error: No video created! Is rtspd running?\n");
        return EXIT_FAILURE;
    }

    // * Retrieve filename of last created video
    int ret = get_last_video();

    return ret;
}
