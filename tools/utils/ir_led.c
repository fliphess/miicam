#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_pwm.h"


static void print_usage(void)
{
    printf("Usage:\n");
    printf("   ir_led [-e|-d|-s|-j]\n");
    printf(
        "\nAvailable options:\n"
        "  -e    enable\n"
        "  -d    disable\n"
        "  -s    status\n"
        "  -i    info\n"
        "  -j    json info\n"
    );

    exit(EXIT_FAILURE);
}


struct CommandLineArguments
{
    unsigned int enable;
    unsigned int disable;
    unsigned int status;
    unsigned int info;
    unsigned int json;
} cli = {0, 0, 0, 0, 0};


int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "edsji")) != -1) {
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
        case 'i':
            cli.info = 1;
            break;
        case 'j':
            cli.json = 1;
            break;
        default:
            fprintf(stderr, "*** Error: unknown option: -%c\n", optopt);
            print_usage();
            break;
        }
    }

    if (!cli.enable && !cli.disable && !cli.status && !cli.json && !cli.info) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((cli.enable + cli.disable + cli.status + cli.json + cli.info > 1)) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (pwm_init() < 0) {
        fprintf(stderr, "*** Error: PWM initialization failed\n");
        return EXIT_FAILURE;
    }

    int success;
    if (cli.enable)
        success = ir_led_on();
    else if (cli.disable)
        success = ir_led_off();
    else if (cli.status)
        success = ir_led_status();
    else if (cli.info)
        success = ir_led_info();
    else if (cli.json)
        success = ir_led_info_json();

    pwm_end();
    return success;
}

