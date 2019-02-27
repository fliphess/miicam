#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "chuangmi_isp328.h"


struct CommandLineArguments
{
    unsigned int ev_value;
    unsigned int delay;
    unsigned int verbose;
} cli = {0, 0, 0};


struct State
{
    unsigned int ev_value;
} state = {0};


static void print_usage(void)
{
    printf("Usage:\n");
    printf("   auto_night_mode [-d|-e|-v]\n");
    printf(
        "\nAvailable options:\n"
        "  -d  (int)  delay in seconds (default: 10)\n"
        "  -e  (int)  lowest EV value\n"
        "  -v  be verbose\n"
    );

    exit(EXIT_FAILURE);
}

void enable_nightmode(void)
{
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning on nightmode\n");

    system("/tmp/sd/firmware/bin/nightmode -e");
}

void disable_nightmode(void)
{
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning off nightmode\n");
    system("/tmp/sd/firmware/bin/nightmode -d");
}

void enable_led(void)
{
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning off IR led\n");

    system("/tmp/sd/firmware/bin/ir_led -e");
}

void disable_led(void)
{
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning off IR led\n");

    system("/tmp/sd/firmware/bin/ir_led -d");
}

void signal_handler(int sig)
{
    if (cli.verbose == 1)
        fprintf(stderr, "*** Exiting auto_night_mode: CTRL+C pressed, or exit requested\n");

    isp328_end();

    exit(0);
}

int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "d:e:v")) != -1) {
        switch (opt)
        {
            case 'd':
                cli.delay = atoi(optarg);
                break;
            case 'e':
                cli.ev_value = atoi(optarg);
                break;
            case 'v':
                cli.verbose = 1;
                break;
            default:
                fprintf(stderr, "*** Error: unknown option: %c\n", optopt);
                print_usage();
                break;
        }
    }

    if (!cli.ev_value) {
        print_usage();
        return -1;
    }

    // * Don't allow a delay below 3 (because we <3 delay)
    if (cli.delay < 3)
        cli.delay = 3;

    if (isp328_init() < 0) {
        fprintf(stderr, "*** Error: ISP328 initialization failed");
        return -1;
    }

    // * Catch all signals
    signal(SIGINT,  signal_handler);
    signal(SIGHUP,  signal_handler);
    signal(SIGTERM, signal_handler);

    if (cli.verbose == 1)
        fprintf(stderr, "*** Auto nightmode started\n");

    while(1) {
        if (nightmode_update_values() < 0) {
            fprintf(stderr, "*** Failed to retrieve EV and IR values!\n");
            sleep(5);
        }

        if (state.ev_value != light_info.ev) {
            if (cli.verbose == 1)
                fprintf(stderr, "*** Nightmode values changed: ev=%d\n", light_info.ev);
        }

        state.ev_value = light_info.ev;

        // * Check EV values and switch on night mode
        if (light_info.ev < cli.ev_value) {
            if (cli.verbose == 1)
                fprintf(stderr, "*** Enable night mode triggered: ev=(%d,%d)\n", light_info.ev, cli.ev_value);

            enable_nightmode();
            enable_led();
        }
        else if (light_info.ev >= cli.ev_value) {
            if (cli.verbose == 1)
                fprintf(stderr, "*** Disable night mode triggered: ev=(%d,%d)\n", light_info.ev, cli.ev_value);

            disable_nightmode();
            disable_led();
        }

        sleep(cli.delay);
    }

    return 0;
}
