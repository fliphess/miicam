#ifndef chuangmi_utils_h
#define chuangmi_utils_h

#define RTSPD_REQUEST_SNAPSHOT "/dev/shm/rtspd_snapshot"
#define RTSPD_LAST_SNAPSHOT_PATH "/dev/shm/rtspd_last_snapshot_path"

#define RTSPD_REQUEST_VIDEO "/dev/shm/rtspd_video"
#define RTSPD_LAST_VIDEO_PATH "/dev/shm/rtspd_last_video_path"

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define GPIO_BUFFER_MAX 3
#define GPIO_DIRECTION_MAX 35
#define GPIO_VALUE_MAX 30

int write_file(const char *file_path, char *content);
int read_int(const char *filename);

int wait_for_file_removal(const char * file_path);
int get_last_file(const char * file_path);
int request_media(const char * file_path);

int get_last_snapshot(void);
int get_last_video(void);

int request_snapshot(void);
int request_video(void);

int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_direction(int pin, int dir);
int gpio_read(int pin);
int gpio_write(int pin, int value);

#endif
