#ifndef chuangmi_isp328_h
#define chuangmi_isp328_h

typedef struct _zoom_info{
    int zoom_stage_cnt;     // * Zoom stage count
    int curr_zoom_index;    // * Current zoom stage index
    int curr_zoom_x10;      // * Zoom magnification
} zoom_info_t;

typedef struct _focus_range{
    int min;
    int max;
} focus_range_t;

#define ISP_IOC_SENSOR 's'
#define ISP_IOC_GET_SENSOR_MIRROR       _IOR(ISP_IOC_SENSOR, 6, int)
#define ISP_IOC_SET_SENSOR_MIRROR       _IOW(ISP_IOC_SENSOR, 6, int)

#define ISP_IOC_GET_SENSOR_FLIP         _IOR(ISP_IOC_SENSOR, 7, int)
#define ISP_IOC_SET_SENSOR_FLIP         _IOW(ISP_IOC_SENSOR, 7, int)

#define ISP_IOC_LENS 'l'
#define LENS_IOC_GET_ZOOM_INFO          _IOR(ISP_IOC_LENS, 1, zoom_info_t*)
#define LENS_IOC_SET_ZOOM_MOVE          _IOW(ISP_IOC_LENS, 2, int)
#define LENS_IOC_GET_FOCUS_RANGE        _IOR(ISP_IOC_LENS, 3, focus_range_t *)
#define LENS_IOC_SET_FOCUS_INIT         _IOW(ISP_IOC_LENS, 4, int)
#define LENS_IOC_GET_FOCUS_POS          _IOR(ISP_IOC_LENS, 5, int)
#define LENS_IOC_SET_FOCUS_POS          _IOW(ISP_IOC_LENS, 6, int)
#define LENS_IOC_SET_ZOOM_INIT          _IOW(ISP_IOC_LENS, 7, int)

#define ISP_IOC_AE 'e'
#define AE_IOC_GET_IRIS_ENABLE          _IOR(ISP_IOC_AE, 17, int)
#define AE_IOC_SET_IRIS_ENABLE          _IOW(ISP_IOC_AE, 17, int)
#define AE_IOC_GET_IRIS_PROBE           _IOR(ISP_IOC_AE, 18, int)
#define AE_IOC_SET_IRIS_PROBE           _IOW(ISP_IOC_AE, 18, int)
#define AE_IOC_GET_AUTO_CONTRAST        _IOR(ISP_IOC_AE, 26, int)
#define AE_IOC_SET_AUTO_CONTRAST        _IOW(ISP_IOC_AE, 26, int)

#define ISP_IOC_MW 'm'
#define ISP_IOC_GET_BRIGHTNESS          _IOR(ISP_IOC_MW, 1, int)
#define ISP_IOC_SET_BRIGHTNESS          _IOW(ISP_IOC_MW, 1, int)
#define ISP_IOC_GET_CONTRAST            _IOR(ISP_IOC_MW, 2, int)
#define ISP_IOC_SET_CONTRAST            _IOW(ISP_IOC_MW, 2, int)
#define ISP_IOC_GET_HUE                 _IOR(ISP_IOC_MW, 3, int)
#define ISP_IOC_SET_HUE                 _IOW(ISP_IOC_MW, 3, int)
#define ISP_IOC_GET_SATURATION          _IOR(ISP_IOC_MW, 4, int)
#define ISP_IOC_SET_SATURATION          _IOW(ISP_IOC_MW, 4, int)
#define ISP_IOC_GET_DENOISE             _IOR(ISP_IOC_MW, 5, int)
#define ISP_IOC_SET_DENOISE             _IOW(ISP_IOC_MW, 5, int)
#define ISP_IOC_GET_SHARPNESS           _IOR(ISP_IOC_MW, 6, int)
#define ISP_IOC_SET_SHARPNESS           _IOW(ISP_IOC_MW, 6, int)
#define ISP_IOC_GET_GAMMA_TYPE          _IOR(ISP_IOC_MW, 7, int)
#define ISP_IOC_SET_GAMMA_TYPE          _IOW(ISP_IOC_MW, 7, int)
#define ISP_IOC_GET_DR_MODE             _IOR(ISP_IOC_MW, 8, int)
#define ISP_IOC_SET_DR_MODE             _IOW(ISP_IOC_MW, 8, int)

#define ISP_IOC_GET_DAYNIGHT_MODE       _IOR(0x6d, 0x0a, int)
#define ISP_IOC_SET_DAYNIGHT_MODE       _IOW(0x6d, 0x0a, int)

#define ISP_DEV_NAME "/dev/isp328"

struct isp_light_info {
    int ev;
    int ir;
};

struct isp_light_info light_info = {0,0};

int isp328_init(void);
int isp328_end(void);
int isp328_is_initialized(void);

int mirrormode_set(int value);
int mirrormode_on(void);
int mirrormode_off(void);
int mirrormode_status(void);

int nightmode_set(int value);
int nightmode_update_values(void);

int nightmode_on(void);
int nightmode_off(void);
int nightmode_status(void);
int nightmode_info(void);
int nightmode_info_json(void);

int flipmode_set(int value);
int flipmode_on(void);
int flipmode_off(void);
int flipmode_status(void);

int brightness_set(int value);
int brightness_reset(void);
int brightness_get(void);
int brightness_print(void);

int contrast_set(int value);
int contrast_reset(void);
int contrast_get(void);
int contrast_print(void);

int hue_set(int value);
int hue_reset(void);
int hue_get(void);
int hue_print(void);

int saturation_set(int value);
int saturation_reset(void);
int saturation_get(void);
int saturation_print(void);

int denoise_set(int value);
int denoise_reset(void);
int denoise_get(void);
int denoise_print(void);

int sharpness_set(int value);
int sharpness_reset(void);
int sharpness_get(void);
int sharpness_print(void);

int print_camera_info_json(void);
int print_camera_info_shell(void);
int print_camera_info(void);

int reset_camera_adjustments(void);

#endif
