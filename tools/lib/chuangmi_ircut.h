#ifndef chuangmi_ircut_h
#define chuangmi_ircut_h

#define GPIO_PIN_14 14
#define GPIO_PIN_15 15

int ircut_init(void);
int ircut_end(void);
int ircut_is_initialized(void);
int ircut_status(void);
int ircut_status_json(void);
int ircut_on(void);
int ircut_off(void);

#endif
