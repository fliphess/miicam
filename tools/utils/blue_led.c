#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_led.h"


struct CommandLineArguments
{
    unsigned int enable;
    unsigned int disable;
    unsigned int blink;
    unsigned int status;
    unsigned int json;
} cli = {0, 0, 0, 0, 0};


static void print_usage(void)
{
    printf("Usage:\n");
    printf("   blue_led [-e|-d|-b|-s|-j]\n");
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

    while ((opt = getopt(argc, argv, "edbsj")) != -1) {
        switch (opt)
        {
        case 'e':
            cli.enable = 1;
            break;
        case 'd':
            cli.disable = 1;
            break;
        case 'b':
            cli.blink = 1;
            break;
        case 's':
            cli.status = 1;
            break;
        case 'j':
            cli.json = 1;
            break;
        default:
            fprintf(stderr, "*** Error: unknown option: %c\n", optopt);
            print_usage();
            break;
        }
    }

    if (!cli.enable && !cli.disable && !cli.blink && !cli.status && !cli.json) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((cli.enable + cli.disable + cli.blink + cli.status + cli.json > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    int success = 0;

    if (cli.enable)
        success = blue_led_on();

    else if (cli.disable)
        success = blue_led_off();

    else if (cli.blink)
        success = blue_led_blink_on();

    else if (cli.status)
        success = blue_led_status();

    else if (cli.json)
        success = blue_led_status_json();

    else
        print_usage();

    return success;
}

