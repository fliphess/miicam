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
    /*
     * Enable GPIO pins
     */
    if (gpio_export(GPIO_PIN_14) == EXIT_FAILURE || gpio_export(GPIO_PIN_15) == EXIT_FAILURE)
        fprintf(stderr, "*** Error: Failed to export ir cut gpio pins!\n");
        return EXIT_FAILURE;

    /*
     * Set gpio_ directions
     */
    if (gpio_direction(GPIO_PIN_14, OUT) == EXIT_FAILURE || gpio_direction(GPIO_PIN_15, OUT) == EXIT_FAILURE)
        fprintf(stderr, "*** Error: Failed to set directions for ir cut gpio pins!\n");
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}


/*
 *  Close/shutdown the ircut lib
 */
int ircut_end(void)
{
    if (gpio_unexport(GPIO_PIN_14) == EXIT_FAILURE) {
        fprintf(stderr, "*** Error: Failed to unexport %s", GPIO_PIN_14);
        return EXIT_FAILURE;
    }

    if (gpio_unexport(GPIO_PIN_15) == EXIT_FAILURE) {
        fprintf(stderr, "*** Error: Failed to unexport %s", GPIO_PIN_15);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
 * Return the state of the ir_cut sensor flag
 * Return value:
 *  -1: Unknown
 *   0: Off
 *   1: On
 */
int ir_cut_get_status(void) 
{
    int pin_0_status = gpio_read(GPIO_PIN_14);
    int pin_1_status = gpio_read(GPIO_PIN_15);

    if (pin_0_status == 0 && pin_1_status == 1)
        return 1;
    else if (pin_0_status == 1 && pin_1_status == 0)
        return 0;
    else {
        return -1;
    }
}


/*
 *  Show if ircut is on or off
 */
int ircut_status(void)
{
    int status = ir_cut_get_status();

    if (status == 1) 
        fprintf(stdout, "*** IR Cut is on\n");

    else if (status == 0)
        fprintf(stdout, "*** IR Cut is off\n");

    else {
        fprintf(stdout, "*** IR Cut is UNKNOWN");
        return EXIT_FAILURE;
    }

    return(EXIT_SUCCESS);
}


/*
 *  Show if ircut is on or off in json
 */
int ircut_status_json(void)
{
    int status = ir_cut_get_status();

    if (status == 1) 
        fprintf(stdout, "{\"ir_cut\":1}");

    else if (status == 0)
        fprintf(stdout, "{\"ir_cut\":0}");

    else {
        fprintf(stdout, "{\"error\":1}");
        return EXIT_FAILURE;
    }

    return(EXIT_SUCCESS);
}


/*
 * Turn on IR Cut
 */
int ircut_on(void)
{
    if (gpio_write(GPIO_PIN_14, 0) == EXIT_FAILURE || gpio_write(GPIO_PIN_15, 1) == EXIT_FAILURE) {
        fprintf(stderr, "*** Error: Failed write to gpio pins!\n");
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}

/*
 * Turn off IR Cut
 */
int ircut_off(void)
{
    if (gpio_write(GPIO_PIN_14, 1) == EXIT_FAILURE || gpio_write(GPIO_PIN_15, 0) == EXIT_FAILURE) {
        fprintf(stderr, "*** Error: Failed write to gpio pins!\n");
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
