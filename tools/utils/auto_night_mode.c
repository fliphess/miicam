#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "chuangmi_isp328.h"


struct CommandLineArguments {
    unsigned int ev_value;
    unsigned int ir_value;
    unsigned int ir_led;
    unsigned int nightmode;
    unsigned int ir_cut;
    unsigned int delay;
    unsigned int verbose;
} cli = {0, 0, 0, 0, 0, 0, 0};


struct State {
    unsigned int ir_led;
    unsigned int nightmode;
    unsigned int ir_cut;
    unsigned int ev_value;
    unsigned int ir_value;
} state = {0, 0, 0, 0, 0};


static void print_usage(void)
{
    printf("Usage:\n");
    printf("   auto_night_mode [-d|-e|-i|-l|-n|-v]\n");
    printf(
        "\nAvailable options:\n"
        "  -d  (int)  delay in seconds (default: 10)\n"
        "  -e  (int)  lowest EV value\n"
        "  -i  (int)  highest IR value\n"
        "  -l  (bool) switch IR led\n"
        "  -n  (bool) switch night mode\n"
        "  -c  (bool) switch IR Cut\n"
        "  -v  be verbose\n"
    );

    exit(EXIT_FAILURE);
}

void enable_nightmode(void)
{
    state.nightmode = 1;
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning on nightmode\n");

    system("/tmp/sd/firmware/bin/nightmode -e");
}

void disable_nightmode(void)
{
    state.nightmode = 0;
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning off nightmode\n");

    system("/tmp/sd/firmware/bin/nightmode -d");
}

void enable_led(void)
{
    state.ir_led = 1;
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning off IR led\n");

    system("/tmp/sd/firmware/bin/ir_led -e");
}

void disable_led(void)
{
    state.ir_led = 0;
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning off IR led\n");

    system("/tmp/sd/firmware/bin/ir_led -d");
}

void enable_ircut(void)
{
    state.ir_cut = 0;
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning on IR cut\n");

    system("/tmp/sd/firmware/bin/ir_cut -e");
}

void disable_ircut(void)
{
    state.ir_cut = 0;
    if (cli.verbose == 1)
        fprintf(stderr, "*** Turning off IR cut\n");

    system("/tmp/sd/firmware/bin/ir_cut -d");
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

    while ((opt = getopt(argc, argv, "d:e:i:lcnv")) != -1) {
        switch (opt)
        {
            case 'd':
                cli.delay = atoi(optarg);
                break;
            case 'e':
                cli.ev_value = atoi(optarg);
                break;
            case 'i':
                cli.ir_value = atoi(optarg);
                break;
            case 'l':
                cli.ir_led = 1;
                break;
            case 'n':
                cli.nightmode = 1;
                break;
            case 'c':
                cli.ir_cut = 1;
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

    if (!cli.ev_value && !cli.ir_value && !cli.ir_led && !cli.nightmode && !cli.ir_cut) {
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

        if (state.ev_value != light_info.ev || state.ir_value != light_info.ir) {
            if (cli.verbose == 1)
                fprintf(stderr, "*** Nightmode values changed: ev=%d ir=%d\n", light_info.ev, light_info.ir);
        }

        state.ev_value = light_info.ev;
        state.ir_value = light_info.ir;

        // * Check EV values and switch on night mode
        if (light_info.ev < cli.ev_value) {
            if (!state.nightmode) {

                if (cli.verbose == 1)
                    fprintf(stderr, "*** Enable night mode triggered: ev=(%d,%d)\n", light_info.ev, cli.ev_value);

                if (cli.nightmode)
                    enable_nightmode();
            }
        }
        else if (light_info.ev >= cli.ev_value) {
            if (state.nightmode) {
                if (cli.verbose == 1)
                    fprintf(stderr, "*** Disable night mode triggered: ev=(%d,%d)\n", light_info.ev, cli.ev_value);

                if (cli.nightmode)
                    disable_nightmode();
            }
        }

        // * Check IR values and switch on IR led
        // * Follow requested guidelines:
        // *
        // * When the IR value is over N the IR led is switched ON
        // * When the IR value is below N the IR led is switch OFF
        // *
        // * See: https://github.com/miicam/MiiCam/issues/1

        if (light_info.ir >= cli.ir_value) {
            if (!state.ir_led) {
                if (cli.verbose == 1)
                    fprintf(stderr, "*** Enable IR led triggered: ir=(%d >= %d)\n", light_info.ir, cli.ir_value);

                if (cli.ir_led)
                    enable_led();

                if (cli.ir_cut)
                    disable_ircut();
            }
        }
        else if (light_info.ir < cli.ir_value) {
            if (state.ir_led) {
                if (cli.verbose == 1)
                    fprintf(stderr, "*** Disable IR led triggered: ir=(%d < %d)\n", light_info.ir, cli.ir_value);

                if (cli.ir_led)
                    disable_led();

                if (cli.ir_cut)
                    enable_ircut();
            }
        }

        sleep(cli.delay);
    }

    return 0;
}
