#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <popt.h>


#include "chuangmi_pwm.h"


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
    poptContext pc;
    struct poptOption po[] = {
        {"enable",  'e', POPT_ARG_NONE, &cli.enable,  0, "Enable the IR led",         "Enable"},
        {"disable", 'd', POPT_ARG_NONE, &cli.disable, 0, "Disable the IR led",        "Disable"},
        {"status",  's', POPT_ARG_NONE, &cli.status,  0, "Retrieve the status",       "Led Status"},
        {"info",    'i',  POPT_ARG_NONE, &cli.info,   0, "Retrieve PWM info",         "Info"},
        {"json",    'j', POPT_ARG_NONE, &cli.json,    0, "Retrieve the info in json", "Status Json"},
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

    if (!cli.enable && !cli.disable && !cli.status && !cli.json && !cli.info) {
        poptPrintUsage(pc, stderr, 0);
        return EXIT_FAILURE;
    }

    if ((cli.enable + cli.disable + cli.status + cli.json + cli.info > 1)) {
        poptPrintUsage(pc, stderr, 0);
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

