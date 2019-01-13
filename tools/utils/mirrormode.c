#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_isp328.h"

static void print_usage(void)
{
    printf("Usage:\n");
    printf("   mirrormode [-e|-d|-s]\n");
    printf(
        "\nAvailable options:\n"
        "  -e    enable\n"
        "  -d    disable\n"
        "  -s    status\n"
    );

    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int opt;
    int enable  = 0;
    int disable = 0;
    int status  = 0;

    while ((opt = getopt(argc, argv, "eds")) != -1) {
        switch (opt)
        {
            case 'e':
                enable = 1;
                break;
            case 'd':
                disable = 1;
                break;
            case 's':
                status = 1;
                break;
            default:
                fprintf(stderr, "*** Error: unknown option: %c\n", optopt);
                print_usage();
                break;
        }
    }

    if (!enable && !disable && !status) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((enable + disable + status > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (isp328_init() < 0) {
        fprintf(stderr, "*** Error: IR Cut GPIO initialization failed\n");
        return EXIT_FAILURE;
    }

    int success;
    if (enable)
        success = mirrormode_on();
    else if (disable)
        success = mirrormode_off();
    else if (status)
        success = mirrormode_status();

    isp328_end();
    return success;
}
