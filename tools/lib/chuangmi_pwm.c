/*
 * originally taken from mijia_ctrl
 * by cck56
 */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "chuangmi_pwm.h"

static int pwm_fd;
pwm_info_t pwm[2];


/*
 * Initialize the pwm lib
 */
int pwm_init(void)
{
    system("modprobe ftpwmtmr010");

    if ((pwm_fd = open(PWM_DEVICE_NAME, O_RDWR)) < 0) {
        fprintf(stderr, "*** Error: Failed to open %s\n", PWM_DEVICE_NAME);
        return -1;
    }

    memset(&pwm[0], 0, sizeof(pwm_info_t));

    pwm[0].clksrc = 1;
    pwm[0].mode = PWM_INTERVAL;
    pwm[0].duty_steps = 0xff;
    pwm[0].duty_ratio = 0x7f;
    pwm[0].intr_cnt = 1;
    pwm[0].repeat_cnt = 0x7f;

    memcpy(&pwm[1], &pwm[0], sizeof(pwm_info_t));

    pwm[0].id = 0;
    pwm[1].id = 1;

    int i;
    for (i = 0; i <= 1; i++) {
        ioctl(pwm_fd, PWM_IOCTL_REQUEST,        &pwm[i].id);
        ioctl(pwm_fd, PWM_IOCTL_SET_CLKSRC,     &pwm[i]);
        ioctl(pwm_fd, PWM_IOCTL_SET_MODE,       &pwm[i]);
        ioctl(pwm_fd, PWM_IOCTL_UPDATE,         &pwm[i].id);
        ioctl(pwm_fd, PWM_IOCTL_SET_DUTY_STEPS, &pwm[i]);
    }

    pwm[1].freq = 15000000;         // * Enable clock for MS41909

    ioctl(pwm_fd, PWM_IOCTL_SET_FREQ, &pwm[1]);
    ioctl(pwm_fd, PWM_IOCTL_UPDATE,   &pwm[1].id);
    ioctl(pwm_fd, PWM_IOCTL_START,    &pwm[1].id);

    return 0;
}


/*
 * Verify if the pwm lib is initialized
 */
int pwm_is_initialized(void)
{
    if ( fcntl(pwm_fd, F_GETFD) < 0 ) {
        fprintf(stderr, "*** Error: PWM Library is uninitialized.\n");
        return -1;
    }
    return 0;
}


/*
 * Close/shutdown the pwm lib
 */
int pwm_end(void)
{
    if ( close(pwm_fd) > 0 )
        return 0;
    else
        return -1;
}


/*
 * Control the values of the pwm pins
 */
void pwm_set(unsigned int val)
{
    pwm[0].duty_ratio = val & 0xff;
    ioctl(pwm_fd, PWM_IOCTL_SET_DUTY_RATIO, &pwm[0]);
    ioctl(pwm_fd, PWM_IOCTL_UPDATE, &pwm[0].id);
    ioctl(pwm_fd, PWM_IOCTL_START, &pwm[0].id);
}


/*
 * Set the ir leds to a value (0-255)
 */
int ir_led_set(unsigned int val)
{
    if (pwm_is_initialized() < 0)
        return -1;

    if (val <= 0xff) {
        fprintf(stderr, "*** Setting IR led to %d\n", val);
        pwm_set(val);
        return 0;
    } else
        fprintf(stderr, "*** Error: Use a value in between 0-255\n");
        return -1;
}


/*
 * Turn the IR Led to 255
 */
int ir_led_on(void)
{
    return ir_led_set(255);
}


/*
 * Turn the IR Led to 0
 */
int ir_led_off(void)
{
    // * To disable, we need to run pwm_set(0) twice.....
    pwm_set(0);
    return ir_led_set(0);
}


/*
 * Return 0 if the led is on, -1 if off
 */
int ir_led_status(void)
{
    if (pwm_is_initialized() < 0)
        return -1;

    ioctl(pwm_fd, PWM_IOCTL_GET_INFO, &pwm[0]);

    if (pwm[0].duty_ratio > 0) {
        fprintf(stdout, "*** IR Led is on\n");
        return 0;
    } else {
        fprintf(stdout, "*** IR Led is off\n");
        return -1;
    }
}


/*
 * Print the status of the pwm pins
 */
int ir_led_info(void)
{
    if (pwm_is_initialized() < 0)
        return -1;

    ioctl(pwm_fd, PWM_IOCTL_GET_INFO, &pwm[0]);
    printf("*************************\n");
    printf("PWM %d information:\n", pwm[0].id);
    printf("  Clock Source: %d\n", pwm[0].clksrc);
    printf("  Frequency: %d\n", pwm[0].freq);
    printf("  Step: %d\n", pwm[0].duty_steps);
    printf("  Duty: %d\n", pwm[0].duty_ratio);
    printf("*************************\n");

    return 0;
}


/*
 * Print the status of the pwm pins in json
 */
int ir_led_info_json(void)
{
    if (pwm_is_initialized() < 0)
        return -1;

    ioctl(pwm_fd, PWM_IOCTL_GET_INFO, &pwm[0]);
    printf("{\"id\":%d,\"source\":%d,\"frequency\":%d,\"step\":%d,\"duty\":%d}", pwm[0].id, pwm[0].clksrc, pwm[0].freq, pwm[0].duty_steps, pwm[0].duty_ratio);
    return 0;
}
