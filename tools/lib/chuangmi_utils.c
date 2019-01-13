/*
 * mijia_ctrl
 * by cck56
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chuangmi_utils.h"

int get_last_file(const char * file_path)
{
    char * buffer = 0;
    long length;
    FILE *fh;

    fh = fopen(file_path, "rb");
    if (!fh) {
        fprintf(stderr, "*** Error: Failed to open: %s\n", file_path);
        return EXIT_FAILURE;
    }

    fseek(fh, 0, SEEK_END);
    length = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    buffer = malloc(length);
    if (buffer)
        fread(buffer, 1, length, fh);

    fclose(fh);

    int success = 0;

    if (buffer) {
        buffer[length] = '\0';
        fprintf(stdout, "*** Created: %s\n", buffer);
        success = EXIT_SUCCESS;
    } else {
        fprintf(stderr, "*** Error: Failed to retrieve file from %s.\n", file_path);
        success = EXIT_FAILURE;
    }

    free(buffer);
    return success;
}


int get_last_snapshot(void)
{
    return get_last_file(RTSPD_LAST_SNAPSHOT_PATH);
}


int get_last_video(void)
{
    return get_last_file(RTSPD_LAST_VIDEO_PATH);
}


int request_media(const char * file_path)
{
    FILE *fd;

    fd = fopen(file_path, "w");
    if (!fd) {
        fprintf(stderr, "*** Error: Failed to open: %s\n", file_path);
        return EXIT_FAILURE;
    }

    fprintf(fd, "gimmeh");
    fclose(fd);

    return EXIT_SUCCESS;
}


int request_snapshot(void)
{
    return request_media(RTSPD_REQUEST_SNAPSHOT);
}


int request_video(void)
{
    return request_media(RTSPD_REQUEST_VIDEO);
}


int wait_for_file_removal(const char * file_path)
{
    // * Wait until snapshot file is deleted
    int count = 0;
    int maxcount = 50;

    while (count < maxcount) {
        count++;

        if (access(file_path, F_OK) < 0) {
            break;
        }
        usleep(2000);
    }

    if (access(file_path, F_OK ) < 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}


// ********************************************************* //
// ** GPIO Functions                                      ** //
// ********************************************************* //

int gpio_export(int pin)
{
	char buffer[GPIO_BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, GPIO_BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}


int gpio_unexport(int pin)
{
	char buffer[GPIO_BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, GPIO_BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}


int gpio_direction(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";

	char path[GPIO_DIRECTION_MAX];
	int fd;

	snprintf(path, GPIO_DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}

	close(fd);
	return(0);
}


int gpio_read(int pin)
{
	char path[GPIO_VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-1);
	}

	if (read(fd, value_str, 3) == -1) {
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}

	close(fd);

	return(atoi(value_str));
}


int gpio_write(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[GPIO_VALUE_MAX];
	int fd;

	snprintf(path, GPIO_VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return(EXIT_FAILURE);
	}

	if (write(fd, &s_values_str[LOW == value ? 0 : 1], 1) != 1) {
		fprintf(stderr, "Failed to write value!\n");
		return(EXIT_FAILURE);
	}

	close(fd);
	return(EXIT_SUCCESS);
}
