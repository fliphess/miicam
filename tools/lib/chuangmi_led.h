#ifndef chuangmi_led_h
#define chuangmi_led_h

#define YELLOW_LED_BRIGHTNESS "/sys/class/leds/RED/brightness"
#define YELLOW_LED_TRIGGER    "/sys/class/leds/RED/trigger"
#define YELLOW_LED_DELAY_ON   "/sys/class/leds/RED/delay_on"
#define YELLOW_LED_DELAY_OFF  "/sys/class/leds/RED/delay_off"

#define BLUE_LED_BRIGHTNESS "/sys/class/leds/BLUE/brightness"
#define BLUE_LED_TRIGGER    "/sys/class/leds/BLUE/trigger"
#define BLUE_LED_DELAY_ON   "/sys/class/leds/BLUE/delay_on"
#define BLUE_LED_DELAY_OFF  "/sys/class/leds/BLUE/delay_off"


int blue_led_get_brightness(void);
int blue_led_set_brightness(int value);

int blue_led_blink_on(void);
int blue_led_blink_off(void);

int blue_led_on(void);
int blue_led_off(void);

int blue_led_status(void);
int blue_led_status_json(void);

int yellow_led_get_brightness(void);
int yellow_led_set_brightness(int value);

int yellow_led_blink_on(void);
int yellow_led_blink_off(void);

int yellow_led_on();
int yellow_led_off();

int yellow_led_status(void);
int yellow_led_status_json(void);

int led_status(void);
int led_status_json(void);

#endif


