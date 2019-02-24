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


struct CommandLineArguments
{
    unsigned int enable;
    unsigned int disable;
    unsigned int status;
    unsigned int json;
} cli = {0, 0, 0, 0};


int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "edsj")) != -1) {
        switch (opt)
        {
        case 'e':
            cli.enable = 1;
            break;
        case 'd':
            cli.disable = 1;
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

    if (!cli.enable && !cli.disable && !cli.status && !cli.json) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((cli.enable + cli.disable + cli.status + cli.json > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (ircut_init() < 0) {
        fprintf(stderr, "*** Error: IR Cut GPIO initialization failed\n");
        return EXIT_FAILURE;
    }

    int success = EXIT_SUCCESS;

    if (cli.enable)
        success = ircut_on();
    else if (cli.disable)
        success = ircut_off();
    else if (cli.status)
        success = ircut_status();
    else if (cli.json)
        success = ircut_status_json();
    else
        print_usage();

    return success;
}

