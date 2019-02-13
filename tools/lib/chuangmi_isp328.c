/*
 * mijia_ctrl
 * by cck56
 */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "chuangmi_isp328.h"

// ************************************************************************** //
// ** ISP 328 Functions                                                    ** //
// ************************************************************************** //

int isp_fd;


/*
 * Initialize the ISP328 device
 */
int isp328_init(void)
{
    // * Check if nightmode device can be accessed
    if (access(ISP_DEV_NAME, F_OK) < 0) {
        fprintf(stderr, "*** Error: Failed to access %s\n", ISP_DEV_NAME);
        return -1;
    }

    // * Open ISP328 file descriptor
    if ((isp_fd = open(ISP_DEV_NAME, O_RDWR)) < 0) {
        fprintf(stderr, "*** Error: Failed to open %s\n", ISP_DEV_NAME);
        return -1;
    }

    return 0;
}


/*
 * Return 0 if the isp328 device is initialized else -1
 */
int isp328_is_initialized(void)
{
    if (fcntl(isp_fd, F_GETFD) == -1) {
        fprintf(stderr, "*** Error: ISP328 Library is uninitialized.\n");
        return -1;
    }
    return 0;
}


/*
 * Close the ISP328 device
 */
int isp328_end(void)
{
    if (close(isp_fd) > 0)
        return 0;
    else
        return -1;
}


// ************************************************************************** //
// ** Mirror Mode                                                          ** //
// ************************************************************************** //

/*
 * Set mirror to a value (Valid: 1/0)
 */
int mirrormode_set(int value)
{
    if (isp328_is_initialized() < 0)
        return -1;

    if (value <= 1) {
        fprintf(stderr, "*** Setting mirror to %d\n", value);
        ioctl(isp_fd, ISP_IOC_SET_SENSOR_MIRROR, &value);
        return 0;
    } else {
        fprintf(stderr, "*** Can't set mirror to %d\n", value);
        return -1;
    }
}


/*
 * Turn mirror on
 */
int mirrormode_on(void)
{
    return mirrormode_set(1);
}


/*
 * Turn mirror off
 */
int mirrormode_off(void)
{
    return mirrormode_set(0);
}


/*
 * Get the status for mirror
 */
int mirrormode_status(void)
{
    int mode;

    int ret = ioctl(isp_fd, ISP_IOC_GET_SENSOR_MIRROR, &mode);
    if (ret < 0) {
        fprintf(stdout, "*** Errror: Retrieving mirror mode values failed");
        return -1;
    }

    fprintf(stdout, "*** Mirror mode is: %s\n", (mode == 1) ? "on" : "off");

    return 0;
}


// ************************************************************************** //
// ** Night Mode                                                           ** //
// ************************************************************************** //

/*
 * Set night mode to a value (Valid: 1/0)
 */
int nightmode_set(int value)
{
    if (isp328_is_initialized() < 0)
        return -1;

    if ( value <= 1 ) {
        fprintf(stderr, "*** Setting nightmode to %d\n", value);
        ioctl(isp_fd, _IOW(0x6d, 0x0a, int), &value);
        return 0;
    } else
        return -1;
}


/*
 * Update the values in `isp_light_info` struct
 */
int nightmode_update_values(void)
{
    unsigned int converge = 0, ev = 0, sta_rdy = 0;
    unsigned int awb_sta[10];

    if (isp328_is_initialized() < 0)
        return -1;

    while (converge < 4) {
        ioctl(isp_fd, _IOR(0x65, 0x23, int), &converge);
        sleep(1);
    }

    ioctl(isp_fd, _IOR(0x65, 0x1f, int), &ev);

    while (sta_rdy != 0xf) {
        ioctl(isp_fd, _IOR(0x63, 0x09, int), &sta_rdy);
        usleep(1000);
    }

    ioctl(isp_fd, _IOR(0x68, 0x8a, int), awb_sta);

    light_info.ev = ev;
    light_info.ir = awb_sta[4] / 230400;

    return 0;
}


/*
 * Turn nightmode on
 */
int nightmode_on(void)
{
    return nightmode_set(1);
}


/*
 * Turn nightmode off
 */
int nightmode_off(void)
{
    return nightmode_set(0);
}


/*
 * Get the status for nightmode
 */
int nightmode_status(void)
{
    if (isp328_is_initialized() < 0)
        return -1;

    int mode;
    ioctl(isp_fd, _IOR(0x6d, 0x0a, int), &mode);
    fprintf(stdout, "*** Night mode is: %s\n", (mode == 1) ? "on" : "off");

    return 0;
}


/*
 * Print the light values
 */
int nightmode_info(void)
{
    if (nightmode_update_values() < 0) {
        fprintf(stderr, "*** Failed to retrieve EV and IR values!\n");
        return -1;
    }

    fprintf(stdout, "EV Value: %d\n", light_info.ev);
    fprintf(stdout, "IR Value: %d\n", light_info.ir);

    return 0;
}


/*
 * Print the light values in json
 */
int nightmode_info_json(void)
{
    if (nightmode_update_values() < 0) {
        fprintf(stderr, "*** Failed to retrieve EV and IR values!\n");
        return -1;
    }

    printf("{\"ev\":%d,\"ir\":%d}", light_info.ev, light_info.ir);

    return 0;
}


// ************************************************************************** //
// ** Flip Mode                                                            ** //
// ************************************************************************** //

/*
 * Set flip to a value (Valid: 1/0)
 */
int flipmode_set(int value)
{
    if (isp328_is_initialized() < 0)
        return -1;

    if (value <= 1) {
        fprintf(stderr, "*** Setting flip to %d\n", value);
        ioctl(isp_fd, ISP_IOC_SET_SENSOR_FLIP, &value);
        return 0;
    } else {
        fprintf(stderr, "*** Error: Cannot set flip to %d\n", value);
        return -1;
    }
}


/*
 * Turn flip on
 */
int flipmode_on(void)
{
    return flipmode_set(1);
}


/*
 * Turn flip off
 */
int flipmode_off(void)
{
    return flipmode_set(0);
}


/*
 * Get the status for flip
 */
int flipmode_status(void)
{
    int mode;

    int ret = ioctl(isp_fd, ISP_IOC_GET_SENSOR_FLIP, &mode);
    if (ret < 0) {
        fprintf(stdout, "*** Errror: Retrieving flip mode values failed");
        return -1;
    }

    fprintf(stdout, "*** Flip mode is: %s\n", (mode == 1) ? "on" : "off");

    return 0;
}


// ************************************************************************** //
// ** Brightness                                                           ** //
// ************************************************************************** //

int brightness_set(int value)
{
    if (isp328_is_initialized() < 0)
        return(-1);

    if (value < 0 && value > 255) {
        fprintf(stderr, "*** Error: Cannot set brightness to %d: Use: (0-255)\n", value);
        return(-1);
    }

    fprintf(stderr, "*** Setting brightness to %d\n", value);

    int ret = ioctl(isp_fd, ISP_IOC_SET_BRIGHTNESS, &value);
    if (ret < 0) {
        fprintf(stderr, "*** Error: Cannot set value for brightness!\n");
        return(-1);
    }

    return(0);
}

int brightness_reset(void)
{
    if (brightness_set(128) < 0 ) {
        fprintf(stderr, "*** Error: Failed to reset brightness to it's default value!\n");
        return -1;
    } else {
        fprintf(stderr, "*** Setting brightness has been reset to it's default value (128)\n");
        return 0;
    }
}

int brightness_get(void)
{
    int value;

    if (isp328_is_initialized() < 0)
        return(-1);

    int ret = ioctl(isp_fd, ISP_IOC_GET_BRIGHTNESS, &value);
    if (ret < 0)
        return(-1);

    return value;
}

int brightness_print(void)
{
    int value = brightness_get();
    if (value < 0) {
        fprintf(stderr, "*** Error: Cannot get value for brightness!\n");
        return -1;
    }

    fprintf(stdout, "*** Value for brightness is %d\n", value);
    return(0);
}


// ************************************************************************** //
// ** Contrast                                                             ** //
// ************************************************************************** //

int contrast_set(int value)
{
    if (isp328_is_initialized() < 0)
        return(-1);

    if (value < 0 && value > 255) {
        fprintf(stderr, "*** Error: Cannot set contrast to %d: Use: (0-255)\n", value);
        return(-1);
    }

    fprintf(stderr, "*** Setting contrast to %d\n", value);

    int ret = ioctl(isp_fd, ISP_IOC_SET_CONTRAST, &value);
    if (ret < 0) {
        fprintf(stderr, "*** Error: Cannot set value for contrast!\n");
        return(-1);
    }

    return(0);
}

int contrast_reset(void)
{
    if (contrast_set(128) < 0 ) {
        fprintf(stderr, "*** Error: Failed to reset contrast to it's default value!\n");
        return -1;
    } else {
        fprintf(stderr, "*** Setting contrast has been reset to it's default value (128)\n");
        return 0;
    }
}

int contrast_get(void)
{
    int value;

    if (isp328_is_initialized() < 0)
        return(-1);

    int ret = ioctl(isp_fd, ISP_IOC_GET_CONTRAST, &value);

    if (ret < 0)
        return(-1);

    return value;
}

int contrast_print(void)
{
    int value = contrast_get();

    if (value < 0) {
        fprintf(stderr, "*** Error: Cannot get value for contrast!\n");
        return -1;
    }

    fprintf(stdout, "*** Value for contrast is %d\n", value);
    return(0);
}

// ************************************************************************** //
// ** Hue                                                                  ** //
// ************************************************************************** //

int hue_set(int value)
{
    if (isp328_is_initialized() < 0)
        return(-1);

    if (value < 0 && value > 255) {
        fprintf(stderr, "*** Error: Cannot set hue to %d: Use: (0-255)\n", value);
        return(-1);
    }

    fprintf(stderr, "*** Setting hue to %d\n", value);

    int ret = ioctl(isp_fd, ISP_IOC_SET_HUE, &value);
    if (ret < 0) {
        fprintf(stderr, "*** Error: Cannot set value for hue!\n");
        return(-1);
    }

    return(0);
}

int hue_reset(void)
{
    if (hue_set(128) < 0 ) {
        fprintf(stderr, "*** Error: Failed to reset hue to it's default value!\n");
        return -1;
    } else {
        fprintf(stderr, "*** Setting hue has been reset to it's default value (128)\n");
        return 0;
    }
}

int hue_get(void)
{
    int value;

    if (isp328_is_initialized() < 0)
        return(-1);

    int ret = ioctl(isp_fd, ISP_IOC_GET_HUE, &value);
    if (ret < 0) {
        return(-1);
    }
    return value;
}


int hue_print(void)
{
    int value = hue_get();

    if (value < 0) {
        fprintf(stderr, "*** Error: Cannot get value for hue!\n");
        return -1;
    }

    fprintf(stdout, "*** Value for hue is %d\n", value);
    return(0);
}


// ************************************************************************** //
// ** Saturation                                                           ** //
// ************************************************************************** //

int saturation_set(int value)
{
    if (isp328_is_initialized() < 0)
        return(-1);

    if (value < 0 && value > 255) {
        fprintf(stderr, "*** Error: Cannot set saturation to %d: Use: (0-255)\n", value);
        return(-1);
    }

    fprintf(stderr, "*** Setting saturation to %d\n", value);

    int ret = ioctl(isp_fd, ISP_IOC_SET_SATURATION, &value);
    if (ret < 0) {
        fprintf(stderr, "*** Error: Cannot set value for saturation!\n");
        return(-1);
    }

    return(0);
}

int saturation_reset(void)
{
    if (saturation_set(128) < 0 ) {
        fprintf(stderr, "*** Error: Failed to reset saturation to it's default value!\n");
        return -1;
    } else {
        fprintf(stderr, "*** Setting saturation has been reset to it's default value (128)\n");
        return 0;
    }
}

int saturation_get(void)
{
    int value;

    if (isp328_is_initialized() < 0)
        return(-1);

    int ret = ioctl(isp_fd, ISP_IOC_GET_SATURATION, &value);
    if (ret < 0)
        return(-1);

    return value;
}

int saturation_print(void)
{
    int value = saturation_get();

    if (value < 0) {
        fprintf(stderr, "*** Error: Cannot get value for saturation!\n");
        return -1;
    }

    fprintf(stdout, "*** Value for saturation is %d\n", value);
    return(0);
}


// ************************************************************************** //
// ** Denoise                                                              ** //
// ************************************************************************** //

int denoise_set(int value)
{
    if (isp328_is_initialized() < 0)
        return(-1);

    if (value < 0 && value > 255) {
        fprintf(stderr, "*** Error: Cannot set denoise to %d: Use: (0-255)\n", value);
        return(-1);
    }

    fprintf(stderr, "*** Setting denoise to %d\n", value);

    int ret = ioctl(isp_fd, ISP_IOC_SET_DENOISE, &value);
    if (ret < 0) {
        fprintf(stderr, "*** Error: Cannot set value for denoise!\n");
        return(-1);
    }

    return(0);
}

int denoise_reset(void)
{
    if (denoise_set(128) < 0 ) {
        fprintf(stderr, "*** Error: Failed to reset denoise to it's default value!\n");
        return -1;
    } else {
        fprintf(stderr, "*** Setting denoise has been reset to it's default value (128)\n");
        return 0;
    }
}

int denoise_get(void)
{
    int value;

    if (isp328_is_initialized() < 0)
        return(-1);

    int ret = ioctl(isp_fd, ISP_IOC_GET_DENOISE, &value);
    if (ret < 0)
        return(-1);

    return value;
}

int denoise_print(void)
{
    int value = denoise_get();
    if (value < 0) {
        fprintf(stderr, "*** Error: Cannot get value for denoise!\n");
        return -1;
    }

    fprintf(stdout, "*** Value for denoise is %d\n", value);
    return(0);
}

// ************************************************************************** //
// ** Sharpness                                                            ** //
// ************************************************************************** //

int sharpness_set(int value)
{
    if (isp328_is_initialized() < 0)
        return(-1);

    if (value < 0 && value > 255) {
        fprintf(stderr, "*** Error: Cannot set sharpness to %d: Use: (0-255)\n", value);
        return(-1);
    }

    fprintf(stderr, "*** Setting sharpness to %d\n", value);

    int ret = ioctl(isp_fd, ISP_IOC_SET_SHARPNESS, &value);
    if (ret < 0) {
        fprintf(stderr, "*** Error: Cannot set value for sharpness!\n");
        return(-1);
    }

    return(0);
}

int sharpness_reset(void)
{
    if (sharpness_set(128) < 0 ) {
        fprintf(stderr, "*** Error: Failed to reset sharpness to it's default value!\n");
        return -1;
    } else {
        fprintf(stderr, "*** Setting sharpness has been reset to it's default value (128)\n");
        return 0;
    }
}

int sharpness_get(void)
{
    int value;

    if (isp328_is_initialized() < 0)
        return(-1);

    int ret = ioctl(isp_fd, ISP_IOC_GET_SHARPNESS, &value);
    if (ret < 0) {
        return(-1);
    }

    return value;
}

int sharpness_print(void)
{
    int value = sharpness_get();

    if (value < 0) {
        fprintf(stderr, "*** Error: Cannot get value for sharpness!\n");
        return -1;
    }

    fprintf(stdout, "* Value for sharpness is %d\n", value);
    return(0);
}


// ************************************************************************** //
// ** Status                                                               ** //
// ************************************************************************** //

int print_camera_info_json(void)
{
        fprintf(stdout, "{");
        fprintf(stdout, "\"brightness\":%d,", brightness_get());
        fprintf(stdout, "\"contrast\":%d,", contrast_get());
        fprintf(stdout, "\"hue\":%d,", hue_get());
        fprintf(stdout, "\"saturation\":%d,", saturation_get());
        fprintf(stdout, "\"denoise\":%d,", denoise_get());
        fprintf(stdout, "\"sharpness\":%d", sharpness_get());
        fprintf(stdout, "}");

        return 0;
}


int print_camera_info_shell(void)
{
        fprintf(stdout, "BRIGHTNESS=\"%d\"\n", brightness_get());
        fprintf(stdout, "CONTRAST=\"%d\"\n",   contrast_get());
        fprintf(stdout, "HUE=\"%d\"\n",        hue_get());
        fprintf(stdout, "SATURATION=\"%d\"\n", saturation_get());
        fprintf(stdout, "DENOISE=\"%d\"\n",    denoise_get());
        fprintf(stdout, "SHARPNESS=\"%d\"\n",   sharpness_get());
        return 0;
}


int print_camera_info(void)
{
        fprintf(stdout, "*** Settings:\n");
        fprintf(stdout, "- Brightness: %d\n", brightness_get());
        fprintf(stdout, "- Contrast:   %d\n", contrast_get());
        fprintf(stdout, "- Hue:        %d\n", hue_get());
        fprintf(stdout, "- Saturation: %d\n", saturation_get());
        fprintf(stdout, "- Denoise:    %d\n", denoise_get());
        fprintf(stdout, "- Sharpness:  %d\n", sharpness_get());
        fprintf(stdout, "\n");

        return 0;
}

// ************************************************************************** //
// ** Reset                                                                ** //
// ************************************************************************** //

int reset_camera_adjustments(void)
{
    fprintf(stderr, "*** Resetting all settings to default!\n");

    if (brightness_reset()  < 0 ||
        contrast_reset()    < 0 ||
        hue_set(128)        < 0 ||
        saturation_set(128) < 0 ||
        denoise_set(128)    < 0 ||
        sharpness_set(128)  < 0)
            return -1;

    return 0;
}
