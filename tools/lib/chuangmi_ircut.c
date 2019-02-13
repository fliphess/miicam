/*
 * Originally taken from mijia_ctrl
 * by cck56
 */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_utils.h"
#include "chuangmi_ircut.h"

/*
 *  Initialize the ircut lib
 */
int ircut_init(void)
{
    if (gpio_active(GPIO_PIN_14) < 0) {
        if (gpio_export(GPIO_PIN_14) < 0) {
            fprintf(stderr, "*** Error: Failed to export gpio pin 14!\n");
            return -1;
        }

        if (gpio_direction(GPIO_PIN_14, OUT) < 0) {
            fprintf(stderr, "*** Error: Failed to set directions for gpio pin 14!\n");
            return -1;
        }
    }

    if (gpio_active(GPIO_PIN_15) < 0) {
        if (gpio_export(GPIO_PIN_15) < 0) {
            fprintf(stderr, "*** Error: Failed to export gpio pin 15!\n");
            return -1;
        }

        if (gpio_direction(GPIO_PIN_15, OUT) < 0) {
            fprintf(stderr, "*** Error: Failed to set directions for gpio pin 14!\n");
            return -1;
        }
    }

    return 0;
}


/*
 *  Close/shutdown the ircut lib
 */
int ircut_end(void)
{
    if (gpio_unexport(GPIO_PIN_14) < 0) {
        fprintf(stderr, "*** Error: Failed to unexport %s", GPIO_PIN_14);
        return -1;
    }

    if (gpio_unexport(GPIO_PIN_15) < 0) {
        fprintf(stderr, "*** Error: Failed to unexport %s", GPIO_PIN_15);
        return -1;
    }

    return 0;
}


/*
 *  Show if ircut is on or off
 */
int ircut_status(void)
{
    int pin_14_status = gpio_read(GPIO_PIN_14);
    int pin_15_status = gpio_read(GPIO_PIN_15);

    if (pin_14_status == 1 && pin_15_status == 0) {
        fprintf(stdout, "*** IR Cut is on\n");
    }
    else if (pin_14_status == 0 && pin_15_status == 1) {
        fprintf(stdout, "*** IR Cut is off\n");
    } else {
        fprintf(stdout, "*** IR Cut is UNKNOWN: pin14: %d, pin15: %d\n", pin_14_status, pin_15_status);
        return -1;
    }

    return 0;
}


/*
 *  Show if ircut is on or off in json
 */
int ircut_status_json(void)
{
    int pin_14_status = gpio_read(GPIO_PIN_14);
    int pin_15_status = gpio_read(GPIO_PIN_15);

    if (pin_14_status == 1 && pin_15_status == 0) {
        fprintf(stdout, "{\"ir_cut\":1,\"pin14\":%d,\"pin15\":%d}", pin_14_status, pin_15_status);
    }

    else if (pin_14_status == 0 && pin_15_status == 1) {
        fprintf(stdout, "{\"ir_cut\":0,\"pin14\":%d,\"pin15\":%d}", pin_14_status, pin_15_status);
    }

    else {
        fprintf(stdout, "{\"error\":1,\"pin14\":%d,\"pin15\":%d}", pin_14_status, pin_15_status);
        return -1;
    }

    return 0;
}


/*
 * Turn on IR Cut
 */
int ircut_on(void)
{
    if (gpio_write(GPIO_PIN_14, 1) < 0 || gpio_write(GPIO_PIN_15, 0) < 0) {
        fprintf(stderr, "*** Error: Failed write to gpio pins!\n");
        return -1;
    }

    return 0;
}

/*
 * Turn off IR Cut
 */
int ircut_off(void)
{
    if (gpio_write(GPIO_PIN_14, 0) < 0 || gpio_write(GPIO_PIN_15, 1) < 0) {
        fprintf(stderr, "*** Error: Failed write to gpio pins!\n");
        return -1;
    }

    return 0;
}
