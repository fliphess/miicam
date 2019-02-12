#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_led.h"


static void print_usage(void)
{
    printf("Usage:\n");
    printf("   yellow_led [-e|-d|-b|-s|-j]\n");
    printf(
        "\nAvailable options:\n"
        "  -e    enable\n"
        "  -d    disable\n"
        "  -b    blink\n"
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
    int blink   = 0;
    int status  = 0;
    int json    = 0;

    while ((opt = getopt(argc, argv, "edbsj")) != -1) {
        switch (opt)
        {
        case 'e':
            enable = 1;
            break;
        case 'd':
            disable = 1;
            break;
        case 'b':
            blink = 1;
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

    if (!enable && !disable && !blink && !status && !json) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((enable + disable + blink + status + json > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    int success = EXIT_SUCCESS;

    if (enable)
        success = yellow_led_on();
    else if (disable)
        success = yellow_led_off();
    else if (blink)
        success = yellow_led_blink_on();
    else if (status)
        success = yellow_led_status();
    else if (json)
        success = yellow_led_status_json();
    else
        print_usage();

    return success;
}

