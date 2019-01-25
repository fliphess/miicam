#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_isp328.h"


static void print_usage_and_exit(void)
{
    printf("Usage:\n");
    printf("   camera_adjust [-s|-g] -t [brightness|contrast|hue|saturation|denoise|sharpness]\n");
    printf(
        "\nAvailable options:\n"
        "  -s    set\n"
        "  -g    get\n"
        "  -t    type\n"
        "  -i    info\n"
        "  -j    info (in json)\n"
        "  -k    info (shell)\n"
        "  -r    reset settings\n"
        "\n\n"
        "Description:\n"
        "- Set: Set the value (0-255)\n"
        "- Get: Get the value\n"
        "- Type: The type of setting to get/set the value for\n"
        "\n"
    );

    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int opt;
    int get   = 0;
    int set   = 0;
    int type  = 0;

    int info  = 0;
    int json  = 0;
    int shell = 0;

    int reset = 0;

    char *setting;
    unsigned int value;

    while ((opt = getopt(argc, argv, "gjikrt:s:")) != -1) {
        switch (opt)
        {
            case 'g':
                get = 1;
                break;
            case 's':
                set = 1;
                value = atoi(optarg);
                break;
            case 't':
                type = 1;
                setting = optarg;
                break;
            case 'i':
                info = 1;
                break;
            case 'j':
                json = 1;
                break;
            case 'k':
                shell = 1;
                break;
            case 'r':
                reset = 1;
                break;
            default:
                print_usage_and_exit();
                break;
        }
    }

    if (get + set + info + json + shell + reset == 0) {
        print_usage_and_exit();
    }

    if (get + set + json + info + shell + reset > 1) {
        fprintf(stderr, "Use either -j, -i, -k, -r, -g or -s but not more then one!\n");
        print_usage_and_exit();
    }

    if (isp328_init() < 0) {
        fprintf(stderr, "*** Error: ISP328 initialization failed\n");
        return EXIT_FAILURE;
    }

    if (reset) {
        return reset_camera_adjustments();
    }

    if (info)
        return print_camera_info();

    if (json)
        return print_camera_info_json();

    if (shell)
        return print_camera_info_shell();

    int success;
    if (strcmp(setting, "brightness") == 0) {
        if (set == 1)
            success = brightness_set(value);
        else if (get == 1)
            success = brightness_print();
    }
    else if (strcmp(setting, "contrast") == 0) {
        if (set == 1)
            success = contrast_set(value);
        else if (get == 1)
            success = contrast_print();
    }
    else if (strcmp(setting, "hue") == 0) {
        if (set == 1)
            success = hue_set(value);
        else if (get == 1)
            success = hue_print();
    }
    else if (strcmp(setting, "saturation") == 0) {
        if (set == 1)
            success = saturation_set(value);
        else if (get == 1)
            success = saturation_print();
    }
    else if (strcmp(setting, "denoise") == 0) {
        if (set == 1)
            success = denoise_set(value);
        else if (get == 1)
            success = denoise_print();
    }
    else if (strcmp(setting, "sharpness") == 0) {
        if (set == 1)
            success = sharpness_set(value);
        else if (get == 1)
            success = sharpness_print();
    }
    else {
        fprintf(stderr, "Options for -t are: [brightness|contrast|hue|saturation|denoise|sharpness]!\n");
        print_usage_and_exit();
    }

    isp328_end();
    return success;
}
