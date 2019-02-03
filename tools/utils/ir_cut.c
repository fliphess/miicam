#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_ircut.h"


static void print_usage(void)
{
    printf("Usage:\n");
    printf("   ir_cut [-e|-d|-s]\n");
    printf(
        "\nAvailable options:\n"
        "  -e    enable\n"
        "  -d    disable\n"
        "  -s    status\n"
        "  -j    json status\n"
    );

    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int opt;
    int enable  = 0;
    int disable = 0;
    int status  = 0;
    int json    = 0;

    while ((opt = getopt(argc, argv, "edsj")) != -1) {
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
        case 'j':
            json = 1;
            break;
        default:
            fprintf(stderr, "*** Error: unknown option: %c\n", optopt);
            print_usage();
            break;
        }
    }

    if (!enable && !disable && !status && !json) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((enable + disable + status + json > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (ircut_init() < 0) {
        fprintf(stderr, "*** Error: IR Cut GPIO initialization failed\n");
        return EXIT_FAILURE;
    }

    int success = EXIT_SUCCESS;

    if (enable)
        success = ircut_on();

    else if (disable)
        success = ircut_off();

    else if (status)
        success = ircut_status();

    else if (json)
        success = ircut_status_json();

    else
        print_usage();

    ircut_end();
    return success;
}

