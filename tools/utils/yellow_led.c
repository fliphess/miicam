#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <popt.h>


#include "chuangmi_led.h"


struct CommandLineArguments
{
    unsigned int enable;
    unsigned int disable;
    unsigned int blink;
    unsigned int status;
    unsigned int json;
} cli = {0, 0, 0, 0, 0};

int main(int argc, char *argv[])
{
    poptContext pc;
    struct poptOption po[] = {
        {"enable",  'e', POPT_ARG_NONE, &cli.enable,  0, "Enable the yellow led",          "Enable"},
        {"disable", 'd', POPT_ARG_NONE, &cli.disable, 0, "Disable the yellow led",         "Disable"},
        {"blink",   'b', POPT_ARG_NONE, &cli.blink,   0, "Turn the yellow led to blink",   "Blink"},
        {"status",  's', POPT_ARG_NONE, &cli.status,  0, "Retrieve the status of the led", "Led Status"},
        {"json",    'j', POPT_ARG_NONE, &cli.json,    0, "Retrieve the status in json",    "Status Json"},
        POPT_AUTOHELP
        {NULL}
    };

    pc = poptGetContext(NULL, argc, (const char **)argv, po, 0);
    poptSetOtherOptionHelp(pc, "[ARG...]");

    if (argc < 2) {
        poptPrintUsage(pc, stderr, 0);
        exit(1);
    }

    int val;
    while ((val = poptGetNextOpt(pc)) >= 0) {
    }

    if (val != -1) {
        fprintf(stderr, "%s: %s\n", poptBadOption(pc, POPT_BADOPTION_NOALIAS), poptStrerror(val));
        return 1;
    }

    if ((cli.enable + cli.disable + cli.blink + cli.status + cli.json > 1)) {
        poptPrintUsage(pc, stderr, 0);
        exit(0);
    }

    int success = 0;

    if (cli.enable)
        success = yellow_led_on();

    else if (cli.disable)
        success = yellow_led_off();

    else if (cli.blink)
        success = yellow_led_blink_on();

    else if (cli.status)
        success = yellow_led_status();

    else if (cli.json)
        success = yellow_led_status_json();

    else {
        poptPrintUsage(pc, stderr, 0);
        exit(0);
    }

    return success;
}
