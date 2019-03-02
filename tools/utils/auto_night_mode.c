#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <popt.h>

#include "chuangmi_isp328.h"

struct CommandLineArguments
{
    int ir_on;
    int ir_off;

    int ev_on;
    int ev_off;

    int ir_cut;
    int ir_led;

    int delay;
    int verbose;
} cli = {0, 0, 0, 0, 0, 0, 0, 0};

struct State
{
    unsigned int ev_value;
    unsigned int ir_value;
} state = {0, 0};

void enable_darkness_mode(void)
{
    if (cli.verbose == 1)
        fprintf(stderr, "*** Enabling darkness mode\n");

    system("/tmp/sd/firmware/bin/nightmode -e");

    if (cli.ir_led) system("/tmp/sd/firmware/bin/ir_led -e");
    if (cli.ir_cut) system("/tmp/sd/firmware/bin/ir_cut -d");
}

void disable_darkness_mode(void)
{
    if (cli.verbose == 1)
        fprintf(stderr, "*** Disabling darkness mode\n");

    system("/tmp/sd/firmware/bin/nightmode -d");

    if (cli.ir_led) system("/tmp/sd/firmware/bin/ir_led -d");
    if (cli.ir_cut) system("/tmp/sd/firmware/bin/ir_cut -e");
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
    poptContext pc;
    struct poptOption po[] = {
        {"ir-on",  'p', POPT_ARG_INT, &cli.ir_on,  90,   "IR on",  "The IR on value "},
        {"ir-off", 'q', POPT_ARG_INT, &cli.ir_off, 1000, "IR off", "The IR off value"},
        {"ev-on",  'r', POPT_ARG_INT, &cli.ev_on,  30,   "EV on",  "The EV on value"},
        {"ev-off", 's', POPT_ARG_INT, &cli.ev_off, 100,  "EV off", "The EV off value"},

        {"set-ircut", 'c', POPT_ARG_NONE, &cli.ir_cut, 0, "Set IR Cut", "Set the IR Cut when darkness mode changes"},
        {"set-irled", 'l', POPT_ARG_NONE, &cli.ir_led, 0, "Set IR Led", "Set the IR Led when darkness mode changes"},

        {"verbose",   'v', POPT_ARG_NONE, &cli.verbose, 0, "Be verbose", "Be more verbose"},
        {"delay",     'd', POPT_ARG_INT,  &cli.delay, 0, "Set delay",    "The sleep time between measurements"},
        POPT_AUTOHELP
        {NULL}
    };

    pc = poptGetContext(NULL, argc, (const char **)argv, po, 0);
    poptSetOtherOptionHelp(pc, "[ARG...]");

    if (argc < 2) {
        poptPrintUsage(pc, stderr, 0);
        exit(1);
    }

    // process options and handle each val returned
    int val;
    while ((val = poptGetNextOpt(pc)) >= 0) {
    }

    if (val != -1) {
        fprintf(stderr, "%s: %s\n", poptBadOption(pc, POPT_BADOPTION_NOALIAS), poptStrerror(val));
        return 1;
    }

     // Handle ARG... part of commandline
     while (poptPeekArg(pc) != NULL) {
         char *arg = (char*)poptGetArg(pc);
         printf("poptGetArg returned arg: \"%s\"\n", arg);
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
                fprintf(stderr, "*** New measurements: ev=%d ir=%d\n", light_info.ev, light_info.ir);

            state.ev_value = light_info.ev;
            state.ir_value = light_info.ir;

            int nightmode = nightmode_is_on();

            if (light_info.ev < cli.ev_on && light_info.ir > cli.ir_on && nightmode == 0) {
                enable_darkness_mode();
            }
            else if (light_info.ev > cli.ev_off && light_info.ir < cli.ir_off && nightmode == 1) {
                disable_darkness_mode();
            }
        }

        sleep(cli.delay);
    }

    return 0;
}
