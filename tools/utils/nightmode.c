#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_isp328.h"

static void print_usage(void)
{
    printf("Usage:\n");
    printf("   nightmode [-e|-d|-s]\n");
    printf(
        "\nAvailable options:\n"
        "  -e    enable\n"
        "  -d    disable\n"
        "  -s    status\n"
        "  -i    info\n"
        "  -j    info (json)\n"
    );

    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int opt;
    int enable  = 0;
    int disable = 0;
    int status  = 0;
    int info    = 0;
    int json    = 0;

    while ((opt = getopt(argc, argv, "edsij")) != -1) {
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
            case 'i':
                info = 1;
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

    if (!enable && !disable && !status && !json && !info) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((enable + disable + status + json + info > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (isp328_init() < 0) {
        fprintf(stderr, "*** Error: ISP328 initialization failed\n");
        return EXIT_FAILURE;
    }

    int success;
    if (enable)
        success = nightmode_on();
    else if (disable)
        success = nightmode_off();
    else if (status)
        success = nightmode_status();
    else if (info)
        success = nightmode_info();
    else if (json)
        success = nightmode_info_json();

    isp328_end();
    return success;
}
