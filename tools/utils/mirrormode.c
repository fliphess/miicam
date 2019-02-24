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


struct CommandLineArguments
{
    unsigned int enable;
    unsigned int disable;
    unsigned int status;
} cli = {0, 0, 0};


int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "eds")) != -1) {
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
            default:
                fprintf(stderr, "*** Error: unknown option: %c\n", optopt);
                print_usage();
                break;
        }
    }

    if (!cli.enable && !cli.disable && !cli.status) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((cli.enable + cli.disable + cli.status > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (isp328_init() < 0) {
        fprintf(stderr, "*** Error: IR Cut GPIO initialization failed\n");
        return EXIT_FAILURE;
    }

    int success;
    if (cli.enable)
        success = mirrormode_on();
    else if (cli.disable)
        success = mirrormode_off();
    else if (cli.status)
        success = mirrormode_status();

    isp328_end();
    return success;
}
