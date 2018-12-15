/*
 * mijia_ctrl
 * by cck56
 */

#include <linux/ioctl.h>
#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#define PWM_DEV_NAME                "/dev/ftpwmtmr010"
#define ISP_DEV_NAME                "/dev/isp328"

#define REPLY_WRONG_ARG_NUM         "FAIL,WRONG_ARG_NUM\n"
#define REPLY_INVALID_ARG           "FAIL,INVALID_ARG\n"
#define REPLY_DEV_INIT_FAIL         "FAIL,DEV_INIT_FAIL\n"
#define REPLY_NULL_CMD              "FAIL,NULL_CMD\n"
#define REPLY_INVALID_CMD           "FAIL,INVALID_CMD\n"
#define REPLY_CMD_NOT_FOUND         "FAIL,CMD_NOT_FOUND\n"

#define BUFF_LEN                    1024
#define MAX_ARG_NUM                 (4)

#define PWM_IOC_MAGIC               'p'
#define PWM_IOCTL_REQUEST           _IOW(PWM_IOC_MAGIC, 1, int)
#define PWM_IOCTL_START             _IOW(PWM_IOC_MAGIC, 2, int)
#define PWM_IOCTL_STOP              _IOW(PWM_IOC_MAGIC, 3, int)
#define PWM_IOCTL_GET_INFO          _IOWR(PWM_IOC_MAGIC, 4, pwm_info_t)
#define PWM_IOCTL_SET_CLKSRC        _IOW(PWM_IOC_MAGIC, 5, pwm_info_t)
#define PWM_IOCTL_SET_FREQ          _IOW(PWM_IOC_MAGIC, 6, pwm_info_t)
#define PWM_IOCTL_SET_DUTY_STEPS    _IOW(PWM_IOC_MAGIC, 7, pwm_info_t)
#define PWM_IOCTL_SET_DUTY_RATIO    _IOW(PWM_IOC_MAGIC, 8, pwm_info_t)
#define PWM_IOCTL_SET_MODE          _IOW(PWM_IOC_MAGIC, 9, pwm_info_t)
#define PWM_IOCTL_ENABLE_INTERRUPT  _IOW(PWM_IOC_MAGIC, 10, int)
#define PWM_IOCTL_DISABLE_INTERRUPT _IOW(PWM_IOC_MAGIC, 11, int)
#define PWM_IOCTL_ALL_START         _IO(PWM_IOC_MAGIC, 12)
#define PWM_IOCTL_ALL_STOP          _IO(PWM_IOC_MAGIC, 13)
#define PWM_IOCTL_UPDATE            _IOW(PWM_IOC_MAGIC, 14, int)
#define PWM_IOCTL_ALL_UPDATE        _IO(PWM_IOC_MAGIC, 15)
#define PWM_IOCTL_RELEASE           _IOW(PWM_IOC_MAGIC, 16, int)
#define PWM_IOC_MAXNR               16


enum pwm_set_mode {
    PWM_ONESHOT,
    PWM_INTERVAL,
    PWM_REPEAT,
    PWM_PATTERN,
};


typedef struct pwm_info {
    unsigned int id;
    unsigned int clksrc;
    enum pwm_set_mode mode;
    unsigned int freq;
    unsigned int duty_steps;
    unsigned int duty_ratio;
    unsigned int pattern[4];
    int intr_cnt;
    unsigned short repeat_cnt;
    unsigned char pattern_len;
} pwm_info_t;

pwm_info_t pwm[2];


typedef int (*cmd_func_t)(int argc, char *argv[], char *buf);

typedef struct {
    char *cmd_name;
    cmd_func_t cmd_func;
} cmd_list_t;


int pwm_fd;
int ircut_fd[2];
int isp_fd;


void show_help(void)
{
    fprintf(stdout,
            "Usage:chuangmi_ctrl [COMMAND]\n"
            "The code controls the IR cut filter, IR led, ISP daynight mode and LED indicator.\n"
            "Commands:\n"
            "IRLED,<value>\n"
            "\tvalue: 0 to 255\n"
            "IRCUT,<value>\n"
            "\tvalue: 0 = off, 1 = on\n"
            "DAYNIGHT,<value>\n"
            "\tvalue: 0 = day mode, 1 = night mode\n"
            "LEDSTATUS,<led>,<value>\n"
            "\tled:   0 = blue, 1 = orange\n"
            "\tvalue: 0 = on, 1 = off, 2 = blink\n"
            "GETSTAT\n"
    );
}


void ircut_on(void)
{
    write(ircut_fd[0], "1", 1);
    lseek(ircut_fd[0], 0, SEEK_SET);
    write(ircut_fd[1], "0", 1);
    lseek(ircut_fd[1], 0, SEEK_SET);
}


void ircut_off(void)
{
    write(ircut_fd[0], "0", 1);
    lseek(ircut_fd[0], 0, SEEK_SET);
    write(ircut_fd[1], "1", 1);
    lseek(ircut_fd[1], 0, SEEK_SET);
}


int ircut_init(void)
{
    int gpio_fd;

    if ((gpio_fd = open("/sys/class/gpio/export", O_WRONLY)) >= 0) {
        write(gpio_fd, "14", 3);
        write(gpio_fd, "15", 3);
        close(gpio_fd);
    }

    if (access("/sys/class/gpio/gpio14", F_OK) < 0) {
        fprintf(stderr, "Failed to access /sys/class/gpio/gpio14\n");
        return -1;
    }

    if (access("/sys/class/gpio/gpio15", F_OK) < 0) {
        fprintf(stderr, "Failed to access /sys/class/gpio/gpio15\n");
        return -1;
    }

    if ((ircut_fd[0] = open("/sys/class/gpio/gpio14/value", O_RDWR)) < 0) {
        fprintf(stderr, "Failed to open /sys/class/gpio/gpio14/value\n");
        return -1;
    }

    if ((ircut_fd[1] = open("/sys/class/gpio/gpio15/value", O_RDWR)) < 0) {
        fprintf(stderr, "Failed to open /sys/class/gpio/gpio15/value\n");
        return -1;
    }

    if ((gpio_fd = open("/sys/class/gpio/gpio14/direction", O_WRONLY)) < 0) {
        fprintf(stderr, "Failed to open /sys/class/gpio/gpio14/direction\n");
        return -1;
    }

    write(gpio_fd, "out", 3);
    close(gpio_fd);

    if ((gpio_fd = open("/sys/class/gpio/gpio15/direction", O_WRONLY)) < 0) {
        fprintf(stderr, "Failed to open /sys/class/gpio/gpio15/direction\n");
        return -1;
    }

    write(gpio_fd, "out", 3);
    close(gpio_fd);

    return 0;
}


void ir_led_set(unsigned int val)
{
    pwm[0].duty_ratio = val & 0xff;
    ioctl(pwm_fd, PWM_IOCTL_SET_DUTY_RATIO, &pwm[0]);
    ioctl(pwm_fd, PWM_IOCTL_UPDATE, &pwm[0].id);
    ioctl(pwm_fd, PWM_IOCTL_START, &pwm[0].id);
}


int pwm_init(void)
{
    system("modprobe ftpwmtmr010");

    if ((pwm_fd = open(PWM_DEV_NAME, O_RDWR)) < 0) {
        fprintf(stderr, "Failed to open %s\n", PWM_DEV_NAME);
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
        ioctl(pwm_fd, PWM_IOCTL_REQUEST, &pwm[i].id);
        ioctl(pwm_fd, PWM_IOCTL_SET_CLKSRC, &pwm[i]);
        ioctl(pwm_fd, PWM_IOCTL_SET_MODE, &pwm[i]);
        ioctl(pwm_fd, PWM_IOCTL_UPDATE, &pwm[i].id);
        ioctl(pwm_fd, PWM_IOCTL_SET_DUTY_STEPS, &pwm[i]);
    }

    pwm[1].freq = 15000000; // enable clock for MS41909
    ioctl(pwm_fd, PWM_IOCTL_SET_FREQ, &pwm[1]);
    ioctl(pwm_fd, PWM_IOCTL_UPDATE, &pwm[1].id);
    ioctl(pwm_fd, PWM_IOCTL_START, &pwm[1].id);

    ir_led_set(0);

    return 0;
}



int cmd_irled(int argc, char *argv[], char *buf)
{
    unsigned int val = -1;

    if ( fcntl(pwm_fd, F_GETFD) == -1 ) {
        strcpy(buf, REPLY_DEV_INIT_FAIL);
        return -1;
    }

    if (argc == 2) {
        sscanf(argv[1], "%d", &val);
        fprintf(stdout, "irled val:%d\n", val);
        if (val <= 0xff) {
            ir_led_set(val);
            sprintf(buf, "OK,VAL=%d\n", val);
        } else {
            strcpy(buf, REPLY_INVALID_ARG);
            return -1;
        }
    } else {
        strcpy(buf, REPLY_WRONG_ARG_NUM);
        return -1;
    }

    return 0;
}


int cmd_ircut(int argc, char *argv[], char *buf)
{
    unsigned int val = -1;

    if (( fcntl(ircut_fd[0], F_GETFD) == -1 ) || ( fcntl(ircut_fd[1], F_GETFD) == -1 )) {
        strcpy(buf, REPLY_DEV_INIT_FAIL);
        return -1;
    }

    if (argc == 2) {
        sscanf(argv[1], "%d", &val);
        fprintf(stdout, "ircut val:%d\n", val);
        if (val <= 1) {

            if (val)
                ircut_on();
            else
                ircut_off();

            sprintf(buf, "OK,VAL=%d\n", val);

        } else {
            strcpy(buf, REPLY_INVALID_ARG);
            return -1;
        }
    } else {
        strcpy(buf, REPLY_WRONG_ARG_NUM);
        return -1;
    }
    return 0;
}


int cmd_daynight(int argc, char *argv[], char *buf)
{
    unsigned int val = -1;

    if ( fcntl(isp_fd, F_GETFD) == -1 ) {
        strcpy(buf, REPLY_DEV_INIT_FAIL);
        return -1;
    }

    if (argc == 2) {
        sscanf(argv[1], "%d", &val);
        fprintf(stdout, "daynight mode:%d\n", val);
        if (val <= 1) {
            ioctl(isp_fd, _IOW(0x6d, 0x0a, int), &val);
            sprintf(buf, "OK,VAL=%d\n", val);
        } else {
            strcpy(buf, REPLY_INVALID_ARG);
            return -1;
        }
    } else {
        strcpy(buf, REPLY_WRONG_ARG_NUM);
        return -1;
    }
    return 0;
}


// * Led: 0 = blue, 1 = orange
// * Cmd: 0 = on,  1 = off,  2 = blink

int cmd_ledstatus(int argc, char *argv[], char *buf)
{
    unsigned int led = -1, cmd = -1;
    char cmd_buf[64];

    if (argc == 3) {
        sscanf(argv[1], "%d", &led);
        sscanf(argv[2], "%d", &cmd);
        fprintf(stdout, "ledstatus led: %d cmd: %d\n", led, cmd);
        if (led <= 1 && cmd <= 2) {
            if (access("/mnt/data/miot/ledctl", X_OK) == 0) {
                sprintf(cmd_buf, "/mnt/data/miot/ledctl %d 50 %d 0 0 2", led, cmd);
                fprintf(stdout, "ledstatus cmd: %s\n", cmd_buf);
                system(cmd_buf);
                sprintf(buf, "OK,LED=%d,CMD=%d\n", led, cmd);
            } else {
                strcpy(buf, REPLY_CMD_NOT_FOUND);
                return -1;
            }
        } else {
            strcpy(buf, REPLY_INVALID_ARG);
            return -1;
        }
    } else {
        strcpy(buf, REPLY_WRONG_ARG_NUM);
        return -1;
    }
    return 0;
}


int cmd_isp_sta(int argc, char *argv[], char *buf)
{
    unsigned int converge = 0,
                 ev       = 0,
                 sta_rdy  = 0;

    unsigned int awb_sta[10];

    if ( fcntl(isp_fd, F_GETFD) == -1 ) {
        strcpy(buf, REPLY_DEV_INIT_FAIL);
        return -1;
    }

    while (converge < 4) {
        ioctl(isp_fd, _IOR(0x65, 0x23, int), &converge);
        sleep(1);
    }

    ioctl(isp_fd, _IOR(0x65, 0x1f, int), &ev);

    while (sta_rdy != 0xf) {
        ioctl(isp_fd, _IOR(0x63, 0x09, int), &sta_rdy);
        sleep(1);
    }

    ioctl(isp_fd, _IOR(0x68, 0x8a, int), awb_sta);

    sprintf(buf, "OK,EV=%d,IR=%d\n", ev, awb_sta[4] / 230400);

    return 0;
}


cmd_list_t cmd_list[] =
{
    {"IRLED", cmd_irled},
    {"IRCUT", cmd_ircut},
    {"DAYNIGHT", cmd_daynight},
    {"LEDSTATUS", cmd_ledstatus},
    {"GETSTAT", cmd_isp_sta},
    {NULL, NULL}
};


int run_cmd(char *buf) {
    const char seps[] = " ,\t\n";
    char *token;
    char *arglist[MAX_ARG_NUM + 2];
    int n = 0;
    int rc = 0;

    token = strtok(buf, seps);
    while (token != NULL) {
        arglist[n++] = token;
        token = strtok(NULL, seps);
    }

    if (n == 0) {
        strcpy(buf, REPLY_NULL_CMD);
        return -1;
    } else {
        cmd_list_t *clp = cmd_list;
        while (clp->cmd_name != NULL) {
            if (strcmp(clp->cmd_name, arglist[0]) == 0) {
                rc = clp->cmd_func(n, arglist, buf);
                break;
            }
            clp++;
        }
        if (clp->cmd_name == NULL) {
            strcpy(buf, REPLY_INVALID_CMD);
            return -1;
        }
    }

    return rc;
}


int main(int argc, char *argv[]) {
    char buf[BUFF_LEN];
    int rc = 0;

    if ((argc > 1) && (strcmp(argv[1], "-h")) == 0)  {
        show_help();
        return 0;
    }

    if (argc > 1) {

        // * Initialize IR Cut
        if (ircut_init() < 0) {
            fprintf(stderr, "IRCUT initialization failed\n");
        }

        // * Initialize PWM (IR Led)
        if (pwm_init() < 0) {
            fprintf(stderr, "PWM initialization failed\n");
        }

        // * Initialize isp328
        if ((isp_fd = open(ISP_DEV_NAME, O_RDWR)) < 0) {
            fprintf(stderr, "Failed to open %s\n", ISP_DEV_NAME);
        }

        int i;

        for (i = 1; i < argc; i++) {
            sprintf(buf,"%s %s", buf, argv[i]);
        }

        rc = run_cmd(buf);
        fprintf(stdout, "%s", buf);

        close(pwm_fd);
        close(ircut_fd[0]);
        close(ircut_fd[1]);
        close(isp_fd);

        return rc;

    } else {
        show_help();
        return 1;
    }
}
