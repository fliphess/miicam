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
	if (gpio_export(GPIO_PIN_0) == EXIT_FAILURE || gpio_export(GPIO_PIN_1) == EXIT_FAILURE)
        fprintf(stderr, "*** Error: Failed to export ir cut gpio pins!\n");
		return(EXIT_FAILURE);

	/*
	 * Set gpio_ directions
	 */
	if (gpio_direction(GPIO_PIN_0, OUT) == EXIT_FAILURE || gpio_direction(GPIO_PIN_1, OUT) == EXIT_FAILURE)
        fprintf(stderr, "*** Error: Failed to set directions for ir cut gpio pins!\n");
		return(EXIT_FAILURE);

    return(EXIT_SUCCESS);
}


/*
 *  Close/shutdown the ircut lib
 */
int ircut_end(void)
{
	if (gpio_unexport(GPIO_PIN_0) == EXIT_FAILURE || gpio_unexport(GPIO_PIN_1) == EXIT_FAILURE)
		return(EXIT_FAILURE);

    return(EXIT_SUCCESS);
}


/*
 *  Show if ircut is on or off
 */
int ircut_status(void)
{
    int pin_0_status = gpio_read(GPIO_PIN_0);
    int pin_1_status = gpio_read(GPIO_PIN_1);

    if (pin_0_status == 1 && pin_1_status == 0)
        fprintf(stdout, "*** IR Cut is on\n");

    else if (pin_0_status == 0 && pin_1_status == 1)
        fprintf(stdout, "*** IR Cut is off\n");
    else {
        fprintf(stdout, "*** IR Cut is UNKNOWN: pin 0: %d, ping 1: %d\n", pin_0_status, pin_1_status);
    }

    return(EXIT_SUCCESS);
}


/*
 *  Show if ircut is on or off in json
 */
int ircut_status_json(void)
{
    int pin_0_status = gpio_read(GPIO_PIN_0);
    int pin_1_status = gpio_read(GPIO_PIN_1);

    if (pin_0_status == 1 && pin_1_status == 0)
        fprintf(stdout, "{\"ir_cut\":1,\"pin0\":%d,\"pin1\":%d}", pin_0_status, pin_1_status);

    else if (pin_0_status == 0 && pin_1_status == 1)
        fprintf(stdout, "{\"ir_cut\":0,\"pin0\":%d,\"pin1\":%d}", pin_0_status, pin_1_status);
    else {
        fprintf(stdout, "{\"error\":1,\"pin0\":%d,\"pin1\":%d}", pin_0_status, pin_1_status);
    }
    return(EXIT_SUCCESS);
}


/*
 * Turn on IR Cut
 */
int ircut_on(void)
{
    if (gpio_write(GPIO_PIN_0, 1) == EXIT_FAILURE || gpio_write(GPIO_PIN_1, 0) == EXIT_FAILURE)
        fprintf(stderr, "*** Error: Failed write to gpio pins!\n");
        return(EXIT_FAILURE);

    return(EXIT_SUCCESS);
}

/*
 * Turn off IR Cut
 */
int ircut_off(void)
{
    if (gpio_write(GPIO_PIN_0, 0) == EXIT_FAILURE || gpio_write(GPIO_PIN_1, 1) == EXIT_FAILURE)
        fprintf(stderr, "*** Error: Failed write to gpio pins!\n");
        return(EXIT_FAILURE);

    return(EXIT_SUCCESS);
}
