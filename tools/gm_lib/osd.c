/**
 * @file encode_with_osd.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This source code is encoder with osd sample
 * Copyright (C) 2012 GM Corp. (http://www.grain-media.com)
 *
 * $Revision: 1.61 $
 * $Date: 2014/04/03 06:45:55 $
 *
 */

/**
 * @example encode_with_osd.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include "gmlib.h"

#define BITSTREAM_LEN                       (720 * 1080 * 3 / 2)
#define MAX_BITSTREAM_NUM                   1

#define OSD_PALETTE_COLOR_AQUA              0xCA48CA93        // * YCrYCb
#define OSD_PALETTE_COLOR_BLACK             0x10801080
#define OSD_PALETTE_COLOR_BLUE              0x296e29f0
#define OSD_PALETTE_COLOR_BROWN             0x51A1515B
#define OSD_PALETTE_COLOR_DODGERBLUE        0x693F69CB
#define OSD_PALETTE_COLOR_GRAY              0xB580B580
#define OSD_PALETTE_COLOR_GREEN             0x5151515B
#define OSD_PALETTE_COLOR_KHAKI             0x72897248
#define OSD_PALETTE_COLOR_LIGHTGREEN        0x90229036
#define OSD_PALETTE_COLOR_MAGENTA           0x6EDE6ECA
#define OSD_PALETTE_COLOR_ORANGE            0x98BC9851
#define OSD_PALETTE_COLOR_PINK              0xA5B3A589
#define OSD_PALETTE_COLOR_RED               0x52F0525A
#define OSD_PALETTE_COLOR_SLATEBLUE         0x3D603DA6
#define OSD_PALETTE_COLOR_WHITE             0xEB80EB80
#define OSD_PALETTE_COLOR_YELLOW            0xD292D210

gm_system_t gm_system;
void *groupfd;                  // * Return of gm_new_groupfd()
void *bindfd;                   // * Return of gm_bind()
void *capture_object;
void *encode_object;


void update_osd_font(void)
{
    gm_osd_font_t osd_font;

    // * Setup OSD font
    osd_font.enabled             = 1;
    osd_font.win_idx             = 0;
    osd_font.align_type          = GM_OSD_ALIGN_TOP_LEFT;
    osd_font.x                   = 0;
    osd_font.y                   = 0;
    osd_font.h_words             = 10;              // * The horizontal number of words of OSD window
    osd_font.v_words             = 1;               // * The vertical number of words of OSD window
    osd_font.h_space             = 0;               // * The vertical space between charater and charater
    osd_font.v_space             = 0;               // * The horizontal space between charater and charater

    osd_font.font_index_len      = 10;
    osd_font.font_index[0]       = (unsigned short)'G';
    osd_font.font_index[1]       = (unsigned short)'r';
    osd_font.font_index[2]       = (unsigned short)'a';
    osd_font.font_index[3]       = (unsigned short)'i';
    osd_font.font_index[4]       = (unsigned short)'n';
    osd_font.font_index[5]       = (unsigned short)'M';
    osd_font.font_index[6]       = (unsigned short)'e';
    osd_font.font_index[7]       = (unsigned short)'d';
    osd_font.font_index[8]       = (unsigned short)'i';
    osd_font.font_index[9]       = (unsigned short)'a';

    osd_font.font_alpha          = GM_OSD_FONT_ALPHA_75;
    osd_font.win_alpha           = GM_OSD_FONT_ALPHA_75;
    osd_font.win_palette_idx     = 0;
    osd_font.font_palette_idx    = 1;
    osd_font.priority            = GM_OSD_PRIORITY_MARK_ON_OSD;
    osd_font.smooth.enabled      = 1;
    osd_font.smooth.level        = GM_OSD_FONT_SMOOTH_LEVEL_WEAK;
    osd_font.marquee.mode        = GM_OSD_MARQUEE_MODE_NONE;
    osd_font.border.enabled      = 1;
    osd_font.border.width        = 1;
    osd_font.border.type         = GM_OSD_BORDER_TYPE_WIN;
    osd_font.border.palette_idx  = 3;
    osd_font.font_zoom           = GM_OSD_FONT_ZOOM_NONE;

    gm_set_osd_font(capture_object, &osd_font);         // * Set OSD attribute
}


void update_osd_font2(void)
{
    gm_osd_font2_t osd_font2;

    osd_font2.font_index = (unsigned short *) malloc(10 * sizeof(unsigned short));
    if (osd_font2.font_index == NULL)
    {
        printf("Error to malloc memory!\n");
        exit(0);
    }

    // * Setup OSD font
    osd_font2.enabled            = 1;
    osd_font2.win_idx            = 0;
    osd_font2.align_type         = GM_OSD_ALIGN_TOP_LEFT;
    osd_font2.x                  = 0;
    osd_font2.y                  = 0;
    osd_font2.h_words            = 10;                  // * The horizontal number of words of OSD window
    osd_font2.v_words            = 1;                   // * The vertical number of words of OSD window
    osd_font2.h_space            = 0;                   // * The vertical space between charater and charater
    osd_font2.v_space            = 0;                   // * The horizontal space between charater and charater

    osd_font2.font_index_len     = 10;
    osd_font2.font_index[0]      = (unsigned short)'G';
    osd_font2.font_index[1]      = (unsigned short)'r';
    osd_font2.font_index[2]      = (unsigned short)'a';
    osd_font2.font_index[3]      = (unsigned short)'i';
    osd_font2.font_index[4]      = (unsigned short)'n';
    osd_font2.font_index[5]      = (unsigned short)'M';
    osd_font2.font_index[6]      = (unsigned short)'e';
    osd_font2.font_index[7]      = (unsigned short)'d';
    osd_font2.font_index[8]      = (unsigned short)'i';
    osd_font2.font_index[9]      = (unsigned short)'a';

    osd_font2.font_alpha         = GM_OSD_FONT_ALPHA_75;
    osd_font2.win_alpha          = GM_OSD_FONT_ALPHA_75;
    osd_font2.win_palette_idx    = 0;
    osd_font2.font_palette_idx   = 1;
    osd_font2.priority           = GM_OSD_PRIORITY_MARK_ON_OSD;
    osd_font2.smooth.enabled     = 1;
    osd_font2.smooth.level       = GM_OSD_FONT_SMOOTH_LEVEL_WEAK;
    osd_font2.marquee.mode       = GM_OSD_MARQUEE_MODE_NONE;
    osd_font2.border.enabled     = 1;
    osd_font2.border.width       = 1;
    osd_font2.border.type        = GM_OSD_BORDER_TYPE_WIN;
    osd_font2.border.palette_idx = 3;
    osd_font2.font_zoom          = GM_OSD_FONT_ZOOM_NONE;

    gm_set_osd_font2(capture_object, &osd_font2);       // * Set OSD attribute(…Ë÷√OSD Ù–‘)

    if (osd_font2.font_index != NULL)
        free(osd_font2.font_index);
}


void update_osd_mask(void)
{
    gm_osd_mask_t osd_mask;

    // * Setup OSD mask
    osd_mask.enabled            = 1;
    osd_mask.mask_idx           = 0;
    osd_mask.x                  = 50;
    osd_mask.y                  = 50;
    osd_mask.width              = 80;
    osd_mask.height             = 40;
    osd_mask.alpha              = 5;
    osd_mask.palette_idx        = 2;
    osd_mask.border.type        = GM_OSD_MASK_BORDER_TYPE_TRUE;
    osd_mask.border.width       = 1;
    osd_mask.align_type         = GM_OSD_ALIGN_TOP_LEFT;

    gm_set_osd_mask(capture_object, &osd_mask, GM_ALL_PATH);            // * Set OSD attribute
}


int main(void)
{
    int ch, ret = 0;
    int key;

    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);

    gm_palette_table_t palette={
        palette_table:{OSD_PALETTE_COLOR_AQUA,
                       OSD_PALETTE_COLOR_BLACK,
                       OSD_PALETTE_COLOR_BLUE,
                       OSD_PALETTE_COLOR_BROWN,
                       OSD_PALETTE_COLOR_DODGERBLUE,
                       OSD_PALETTE_COLOR_GRAY,
                       OSD_PALETTE_COLOR_GREEN,
                       OSD_PALETTE_COLOR_KHAKI,
                       OSD_PALETTE_COLOR_LIGHTGREEN,
                       OSD_PALETTE_COLOR_MAGENTA,
                       OSD_PALETTE_COLOR_ORANGE,
                       OSD_PALETTE_COLOR_PINK,
                       OSD_PALETTE_COLOR_RED,
                       OSD_PALETTE_COLOR_SLATEBLUE,
                       OSD_PALETTE_COLOR_WHITE,
                       OSD_PALETTE_COLOR_YELLOW }};

    gm_init();
    gm_get_sysinfo(&gm_system);

    groupfd = gm_new_groupfd();                     // * Create new record group fd

    capture_object = gm_new_obj(GM_CAP_OBJECT);     // * New capture object
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);  // * Create encoder object

    ch                              = 0;            // * Use capture virtual channel 0
    cap_attr.cap_vch                = ch;

    // * GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    // * GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)

    cap_attr.path                   = 2;
    cap_attr.enable_mv_data         = 0;

    gm_set_attr(capture_object, &cap_attr);         // * Set capture attribute

    h264e_attr.dim.width            = gm_system.cap[ch].dim.width;
    h264e_attr.dim.height           = gm_system.cap[ch].dim.height;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode         = GM_EVBR;
    h264e_attr.ratectl.gop          = 60;
    h264e_attr.ratectl.bitrate      = 4096;         // * 2Mbps
    h264e_attr.b_frame_num          = 0;            // * B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data       = 0;            // * Disable H.264 motion data output

    gm_set_attr(encode_object, &h264e_attr);

    // * Bind channel recording
    bindfd = gm_bind(groupfd, capture_object, encode_object);

    if (gm_apply(groupfd) < 0)
    {
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    // * OSD palette config and load logo mark
    ret = gm_set_palette_table(&palette);
    if (ret < 0)
    {
        perror("Set osd palette failed");
        return -1;
    }

    printf("Start Encoding....\n");
    printf("Please select OSD feature:\n"
           "   'f': enable OSD font\n"
           "   '2': enable OSD font2\n"
           "   'm': enable OSD Mask\n"
    );

    while (1)
    {
        key = getchar();

        if (key == 'f')
        {
            printf("Update OSD font...\n");
            update_osd_font();
        }
        else if (key == '2')
        {
            printf("Update OSD font2...\n");
            update_osd_font2();
        }
        else if (key == 'm')
        {
            printf("Mask enabled...\n");
            update_osd_mask();
        }
        else if (key == 'q')
            break;
    }

    gm_unbind(bindfd);
    gm_apply(groupfd);
    gm_delete_obj(encode_object);
    gm_delete_obj(capture_object);
    gm_delete_groupfd(groupfd);
    gm_release();

    return ret;
}
