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
#include "chuangmi_led.h"


// ********************************************************* //
// ** Blue led Functions                                  ** //
// ********************************************************* //

int blue_led_get_brightness(void)
{
    int brightness = read_int(BLUE_LED_BRIGHTNESS);
    if (brightness < 0) {
        return -1;
    }
    return brightness;
}

int blue_led_set_brightness(int value)
{
    char numstring[5];
    snprintf(numstring, sizeof numstring, "%d", value);

    if (write_file(BLUE_LED_BRIGHTNESS, numstring) < 0) {
        return -1;
    }

    return 0;
}

int blue_led_blink_on(void)
{
    if (write_file(BLUE_LED_TRIGGER, "timer\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn on  blue led blink\n");
        return -1;
    }

    if (write_file(BLUE_LED_DELAY_ON, "100\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn on blue led blink\n");
        return -1;
    }

    if (write_file(BLUE_LED_DELAY_OFF, "100\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn on blue led blink\n");
        return -1;
    }

    if (write_file(BLUE_LED_BRIGHTNESS, "50\n") < 0) {
        return -1;
    }

    return 0;
}

int blue_led_blink_off(void)
{
    if (write_file(BLUE_LED_TRIGGER, "none\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn off blue led blink\n");
        return -1;
    }
    return 0;
}

int blue_led_on(void)
{
    if (blue_led_blink_off() < 0)
        return -1;

    int success = blue_led_set_brightness(50);

    if (success < 0) {
        fprintf(stderr, "*** Failed to turn the blue led on\n");
        return -1;
    }

    fprintf(stderr, "*** The blue led was turned on\n");
    return 0;
}

int blue_led_off(void)
{
    if (blue_led_blink_off() < 0)
        return -1;

    if (blue_led_set_brightness(0) < 0) {
        fprintf(stderr, "*** Failed to turn the blue led off\n");
        return -1;
    }

    fprintf(stderr, "*** The blue led was set off\n");
    return 0;
}

int blue_led_status(void)
{
    int brightness = blue_led_get_brightness();

    if (access(BLUE_LED_DELAY_ON, F_OK) == 0 || brightness > 0) {
        fprintf(stdout, "*** Blue led is on\n");
    }
    else if (brightness == 0) {
        fprintf(stdout, "*** Blue led is off\n");
    }
    else {
        fprintf(stdout, "*** Blue led is UNKNOWN: brightness: %d\n", brightness);
        return -1;
    }

    return 0;
}

int blue_led_status_json(void)
{
    int brightness = blue_led_get_brightness();

    if (access(BLUE_LED_DELAY_ON, F_OK) == 0 || brightness > 0) {
        fprintf(stdout, "{\"blue_led\":1,\"brightness\":%d}", brightness);
    }
    else if (brightness == 0) {
        fprintf(stdout, "{\"blue_led\":0,\"brightness\":%d}", brightness);
    }
    else {
        fprintf(stdout, "{\"error\":1,\"brightness\":%d}", brightness);
        return -1;
    }

    return 0;
}


// ********************************************************* //
// ** Yellow led Functions                                ** //
// ********************************************************* //

int yellow_led_get_brightness(void)
{
    int brightness = read_int(YELLOW_LED_BRIGHTNESS);
    if (brightness < 0) {
        return -1;
    }
    return brightness;
}

int yellow_led_set_brightness(int value)
{
    char numstring[5];
    snprintf(numstring, sizeof numstring, "%d", value);

    if (write_file(YELLOW_LED_BRIGHTNESS, numstring) < 0) {
        return -1;
    }

    return 0;
}

int yellow_led_blink_on(void)
{
    if (write_file(YELLOW_LED_TRIGGER, "timer\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn on  yellow led blink\n");
        return -1;
    }

    if (write_file(YELLOW_LED_DELAY_ON, "100\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn on yellow led blink\n");
        return -1;
    }

    if (write_file(YELLOW_LED_DELAY_OFF, "100\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn on yellow led blink\n");
        return -1;
    }

    if (write_file(YELLOW_LED_BRIGHTNESS, "50\n") < 0) {
        return -1;
    }

    return 0;
}

int yellow_led_blink_off(void)
{
    if (write_file(YELLOW_LED_TRIGGER, "none\n") < 0 ) {
        fprintf(stderr, "*** ERROR: Failed to turn off yellow led blink\n");
        return -1;
    }
    return 0;
}

int yellow_led_on()
{
    if (yellow_led_blink_off() < 0)
        return -1;

    int success = yellow_led_set_brightness(50);

    if (success < 0) {
        fprintf(stderr, "*** Failed to turn the yellow led on\n");
        return -1;
    }

    fprintf(stderr, "*** The yellow led was turned on\n");
    return 0;
}

int yellow_led_off()
{
    if (yellow_led_blink_off() < 0)
        return -1;

    if (yellow_led_set_brightness(0) < 0) {
        fprintf(stderr, "*** Failed to turn the yellow led off\n");
        return -1;
    }

    fprintf(stderr, "*** The yellow led was set off\n");
    return 0;
}

int yellow_led_status(void)
{
    int brightness = yellow_led_get_brightness();

    if (access(YELLOW_LED_DELAY_ON, F_OK) == 0 || brightness > 0) {
        fprintf(stdout, "*** Yellow led is on\n");
    }
    else if (brightness == 0) {
        fprintf(stdout, "*** Yellow led is off\n");
    }
    else {
        fprintf(stdout, "*** Yellow led is UNKNOWN: brightness: %d\n", brightness);
        return -1;
    }

    return 0;
}

int yellow_led_status_json(void)
{
    int brightness = yellow_led_get_brightness();

    if (access(YELLOW_LED_DELAY_ON, F_OK) == 0 || brightness > 0) {
        fprintf(stdout, "{\"yellow_led\":1,\"brightness\":%d}", brightness);
    }
    else if (brightness == 0) {
        fprintf(stdout, "{\"yellow_led\":0,\"brightness\":%d}", brightness);
    }
    else {
        fprintf(stdout, "{\"error\":1,\"brightness\":%d}", brightness);
        return -1;
    }

    return 0;
}


// ********************************************************* //
// ** Combined Led Functions                              ** //
// ********************************************************* //

int led_status(void)
{
    int yellow = yellow_led_get_brightness();
    int blue = blue_led_get_brightness();

    if ((blue < 0) || (yellow < 0)) {
        fprintf(stderr, "*** ERROR: Failed to retrieve the brightness of both leds");
        return -1;
    }

    fprintf(stdout, "*** Yellow led is %d\n", yellow);
    fprintf(stdout, "*** Blue led is %d\n", blue);
}

int led_status_json(void)
{
    int yellow = yellow_led_get_brightness();
    int blue = blue_led_get_brightness();

    if ((blue < 0) || (yellow < 0)) {
        fprintf(stderr, "*** ERROR: Failed to retrieve the brightness of both leds");
        return -1;
    }

    fprintf(stdout, "{\"yellow_led\":%s,\"blue_led\":%d}", yellow, blue);
    return 0;
}
