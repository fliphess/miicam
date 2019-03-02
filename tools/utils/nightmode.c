#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <popt.h>

#include "chuangmi_isp328.h"


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
        {"enable",  'e', POPT_ARG_NONE, &cli.enable,  0, "Enable the Nightmode",      "Enable"},
        {"disable", 'd', POPT_ARG_NONE, &cli.disable, 0, "Disable the Nightmode",     "Disable"},
        {"status",  's', POPT_ARG_NONE, &cli.status,  0, "Retrieve the status",       "Status"},
        {"info",    'i', POPT_ARG_NONE, &cli.info,    0, "Retrieve nightmode info",   "Nightmode Info"},
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
        exit(1);
    }

    if ((cli.enable + cli.disable + cli.status + cli.json + cli.info > 1)) {
        poptPrintUsage(pc, stderr, 0);
        exit(1);
    }

    if (isp328_init() < 0) {
        fprintf(stderr, "*** Error: ISP328 initialization failed\n");
        return EXIT_FAILURE;
    }

    int success;
    if (cli.enable)
        success = nightmode_on();
    else if (cli.disable)
        success = nightmode_off();
    else if (cli.status)
        success = nightmode_status();
    else if (cli.info)
        success = nightmode_info();
    else if (cli.json)
        success = nightmode_info_json();

    isp328_end();
    return success;
}
