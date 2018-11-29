/**
 * @file encode_with_osd.c
 *
 * Support platform: GM8210, GM8287, GM8139
 * This source code is encoder with osd sample
 * (此示例为开OSD编码程序)
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
#include <pthread.h>
#include <sys/time.h>
#include "gmlib.h"

typedef struct {
    int font;
    char fbitmap[36];   //Font: 12x18, its format is (12bits(MSB) + 4bits_reserve(LSB)) x 18
} font_data_t;

#define FONT_INDEX_STR  0x20

static font_data_t osd_gm_title[] = {    /* GM Chinese Title */
    {
        .font = FONT_INDEX_STR,
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x03, 0xc0,
                     0x01, 0x00, 0x01, 0xf0, 0x01, 0x80, 0x01, 0x80, 0x01,
                     0xf0, 0x00, 0x00, 0x00, 0x60, 0x00, 0xe0, 0x03, 0xc0, },
    },
    {
        .font = (FONT_INDEX_STR + 1),
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x10, 0x00, 0xfc, 0x00, 0x1c, 0x00,
                     0x18, 0x00, 0xf8, 0x00, 0x18, 0x00, 0x70, 0x00, 0xf0,
                     0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x00, 0x18, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 2),
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10,
                     0x00, 0xf0, 0x30, 0x50, 0x38, 0x00, 0x18, 0x30, 0x00,
                     0x30, 0x00, 0x30, 0x00, 0x10, 0x0c, 0x10, 0x3e, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 3),
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x06, 0x00, 0x86, 0x00, 0x8f, 0x80,
                     0x8c, 0x00, 0x80, 0x00, 0x06, 0x00, 0xff, 0x00, 0x33,
                     0x00, 0xff, 0x00, 0x36, 0x00, 0xfe, 0x00, 0x20, 0x80, },
    },
    {
        .font = (FONT_INDEX_STR + 4),
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0xe0, 0x03, 0xc0, 0x07, 0x00, 0x1f, 0x00, 0x03,
                     0x80, 0x01, 0x00, 0x01, 0xe0, 0x1f, 0xe0, 0x7f, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 5),
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
                     0x03, 0x00, 0x03, 0x00, 0x63, 0x00, 0x33, 0x00, 0x03,
                     0x00, 0x03, 0x00, 0x63, 0x00, 0x33, 0x00, 0x03, 0xe0, },
    },
    {
        .font = (FONT_INDEX_STR + 6),
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
                     0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03,
                     0x00, 0x03, 0xd0, 0x3f, 0x00, 0x03, 0x00, 0x03, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 7),
        .fbitmap = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00,
                     0x38, 0x00, 0x10, 0x00, 0x10, 0x00, 0x11, 0x80, 0x7f,
                     0x80, 0xf0, 0x00, 0x10, 0x00, 0x10, 0x00, 0x17, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 8),
        .fbitmap = { 0x04, 0xc0, 0x00, 0xc0, 0x00, 0xf0, 0x1f, 0xc0, 0x00,
                     0x80, 0x01, 0x80, 0x01, 0x00, 0x03, 0x00, 0x04, 0x00,
                     0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 9),
        .fbitmap = { 0x10, 0x00, 0x1b, 0x00, 0xff, 0xc0, 0x30, 0x00, 0x10,
                     0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
                     0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 0xa),
        .fbitmap = { 0x0c, 0xc0, 0x09, 0xf0, 0x08, 0x40, 0x0c, 0x40, 0x04,
                     0x40, 0x06, 0x40, 0x00, 0x00, 0x7f, 0x00, 0x61, 0xf0,
                     0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 0xb),
        .fbitmap = { 0x7f, 0xc0, 0x20, 0xc0, 0x24, 0xc0, 0x3e, 0xc0, 0xe2,
                     0xc0, 0x00, 0xc0, 0x03, 0x80, 0x01, 0x80, 0x00, 0x00,
                     0xff, 0xe0, 0x7f, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 0xc),
        .fbitmap = { 0x07, 0x00, 0x07, 0xd0, 0x0f, 0x60, 0x0f, 0x00, 0x1b,
                     0x00, 0x33, 0x00, 0x63, 0x00, 0x03, 0x00, 0x03, 0x00,
                     0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 0xd),
        .fbitmap = { 0x3f, 0xe0, 0xc3, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03,
                     0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00,
                     0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 0xe),
        .fbitmap = { 0x03, 0x80, 0x03, 0x10, 0x0e, 0x00, 0x3e, 0x00, 0xf2,
                     0x00, 0x62, 0x00, 0x03, 0x00, 0x03, 0x00, 0x0f, 0x00,
                     0x0e, 0x10, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
    {
        .font = (FONT_INDEX_STR + 0xf),
        .fbitmap = { 0xff, 0x00, 0xc3, 0x00, 0xc6, 0x00, 0x46, 0x00, 0x24,
                     0x00, 0x3c, 0x00, 0x18, 0x00, 0x3c, 0x00, 0x6f, 0x00,
                     0xc7, 0xc0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    },
};

#define LOGO_YUV422_PATTERN     "logo_32x32_yuv422.bin"
#define LOGO_WIDTH              32
#define LOGO_HEIGHT             32

#define OSG_LOGO_RGB1555_PATTERN     "osg_16x72_rgb1555.bin"
#define OSG_LOGO_WIDTH               16
#define OSG_LOGO_HEIGHT              72

#define BITSTREAM_LEN           (720 * 576 * 3 / 2)
#define MAX_BITSTREAM_NUM   1

#define OSD_PALETTE_COLOR_AQUA              0xCA48CA93        /* YCrYCb */
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
void *groupfd;   // return of gm_new_groupfd()
void *bindfd;    // return of gm_bind()
void *capture_object;
void *encode_object;
pthread_t enc_thread_id;
int enc_exit = 0;   // Notify program exit

static void *encode_thread(void *arg)
{
    int i, ret;
    int ch = (int)arg;
    FILE *record_file;
    char filename[50];
    char *bitstream_data;
    gm_pollfd_t poll_fds[MAX_BITSTREAM_NUM];
    gm_enc_multi_bitstream_t multi_bs[MAX_BITSTREAM_NUM];

    // open file for write(打开录相文件)
    bitstream_data = (char *)malloc(BITSTREAM_LEN);
    memset(bitstream_data, 0, BITSTREAM_LEN);

    sprintf(filename, "ch%d_%dx%d_osd.264", ch, gm_system.cap[ch].dim.width,
            gm_system.cap[ch].dim.height);
    printf("Create %s bitstream...\n", filename);
    record_file = fopen(filename, "wb");
    if (record_file == NULL) {
        printf("open file error(%s)! \n", filename);
        exit(1);
    }

    memset(poll_fds, 0, sizeof(poll_fds));

    poll_fds[ch].bindfd = bindfd;
    poll_fds[ch].event = GM_POLL_READ;
    while (enc_exit == 0) {
        /** poll bitstream until 500ms timeout */
        ret = gm_poll(poll_fds, MAX_BITSTREAM_NUM, 500);
        if (ret == GM_TIMEOUT) {
            printf("Poll timeout!!");
            continue;
        }

        memset(multi_bs, 0, sizeof(multi_bs));  //clear all mutli bs
        for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
            if (poll_fds[i].revent.event != GM_POLL_READ)
                continue;
            if (poll_fds[i].revent.bs_len > BITSTREAM_LEN) {
                printf("bitstream buffer length is not enough! %d, %d\n",
                    poll_fds[i].revent.bs_len, BITSTREAM_LEN);
                continue;
            }
            multi_bs[i].bindfd = poll_fds[i].bindfd;
            multi_bs[i].bs.bs_buf = bitstream_data;  // set buffer point(指定输出缓冲指针位置)
            multi_bs[i].bs.bs_buf_len = BITSTREAM_LEN;  // set buffer length(指定缓冲长度)
            multi_bs[i].bs.mv_buf = 0;  // not to recevie MV data
            multi_bs[i].bs.mv_buf_len = 0;  // not to recevie MV data
        }
        
        if ((ret = gm_recv_multi_bitstreams(multi_bs, MAX_BITSTREAM_NUM)) < 0) {
            printf("Error return value %d\n", ret);
        } else {
            for (i = 0; i < MAX_BITSTREAM_NUM; i++) {
                if ((multi_bs[i].retval < 0) && multi_bs[i].bindfd) {
                    printf("CH%d Error to receive bitstream. ret=%d\n", i, multi_bs[i].retval);
                } else if (multi_bs[i].retval == GM_SUCCESS) {
#if 0
                    printf("<CH%d, mv_len=%d bs_len=%d, keyframe=%d, newbsflag=0x%x>\n",
                            i, multi_bs[i].bs.mv_len, multi_bs[i].bs.bs_len,
                            multi_bs[i].bs.keyframe, multi_bs[i].bs.newbs_flag);
#endif
                    fwrite(multi_bs[i].bs.bs_buf,1, multi_bs[i].bs.bs_len, record_file);
                    fflush(record_file);
                }
            }
        }
    }

    fclose(record_file);
    free(bitstream_data);
    return 0;
}

void update_osd_font(void)
{
    gm_osd_font_t osd_font;

    /** setup OSD font */   
    osd_font.enabled = 1;
    osd_font.win_idx = 0;
    osd_font.align_type = GM_OSD_ALIGN_TOP_LEFT;
    osd_font.x = 0;
    osd_font.y = 0;
    osd_font.h_words = 10;  ///< The horizontal number of words of OSD window
    osd_font.v_words = 1;   ///< The vertical number of words of OSD window
    osd_font.h_space = 0;   ///< The vertical space between charater and charater
    osd_font.v_space = 0;   ///< The horizontal space between charater and charater

    osd_font.font_index_len = 10;
    osd_font.font_index[0] = (unsigned short)'G';
    osd_font.font_index[1] = (unsigned short)'r';
    osd_font.font_index[2] = (unsigned short)'a';
    osd_font.font_index[3] = (unsigned short)'i';
    osd_font.font_index[4] = (unsigned short)'n';
    osd_font.font_index[5] = (unsigned short)'M';
    osd_font.font_index[6] = (unsigned short)'e';
    osd_font.font_index[7] = (unsigned short)'d';
    osd_font.font_index[8] = (unsigned short)'i';
    osd_font.font_index[9] = (unsigned short)'a';
    
    osd_font.font_alpha = GM_OSD_FONT_ALPHA_75;
    osd_font.win_alpha = GM_OSD_FONT_ALPHA_75;
    osd_font.win_palette_idx = 0;
    osd_font.font_palette_idx = 1;
    osd_font.priority = GM_OSD_PRIORITY_MARK_ON_OSD;
    osd_font.smooth.enabled = 1;
    osd_font.smooth.level = GM_OSD_FONT_SMOOTH_LEVEL_WEAK;
    osd_font.marquee.mode = GM_OSD_MARQUEE_MODE_NONE;
    osd_font.border.enabled = 1;
    osd_font.border.width = 1;
    osd_font.border.type = GM_OSD_BORDER_TYPE_WIN;
    osd_font.border.palette_idx = 3;
    osd_font.font_zoom = GM_OSD_FONT_ZOOM_NONE;
    gm_set_osd_font(capture_object, &osd_font);//set OSD attribute(设置OSD属性)
}

void update_osd_font2(void)
{
    gm_osd_font2_t osd_font2;

    osd_font2.font_index = (unsigned short *) malloc(10 * sizeof(unsigned short));
    if (osd_font2.font_index == NULL) {
        printf("Error to malloc memory!\n");
        exit(0);
    }

    /** setup OSD font */   
    osd_font2.enabled = 1;
    osd_font2.win_idx = 0;
    osd_font2.align_type = GM_OSD_ALIGN_TOP_LEFT;
    osd_font2.x = 0;
    osd_font2.y = 0;
    osd_font2.h_words = 10;  ///< The horizontal number of words of OSD window
    osd_font2.v_words = 1;   ///< The vertical number of words of OSD window
    osd_font2.h_space = 0;   ///< The vertical space between charater and charater
    osd_font2.v_space = 0;   ///< The horizontal space between charater and charater

    osd_font2.font_index_len = 10;
    osd_font2.font_index[0] = (unsigned short)'G';
    osd_font2.font_index[1] = (unsigned short)'r';
    osd_font2.font_index[2] = (unsigned short)'a';
    osd_font2.font_index[3] = (unsigned short)'i';
    osd_font2.font_index[4] = (unsigned short)'n';
    osd_font2.font_index[5] = (unsigned short)'M';
    osd_font2.font_index[6] = (unsigned short)'e';
    osd_font2.font_index[7] = (unsigned short)'d';
    osd_font2.font_index[8] = (unsigned short)'i';
    osd_font2.font_index[9] = (unsigned short)'a';
    
    osd_font2.font_alpha = GM_OSD_FONT_ALPHA_75;
    osd_font2.win_alpha = GM_OSD_FONT_ALPHA_75;
    osd_font2.win_palette_idx = 0;
    osd_font2.font_palette_idx = 1;
    osd_font2.priority = GM_OSD_PRIORITY_MARK_ON_OSD;
    osd_font2.smooth.enabled = 1;
    osd_font2.smooth.level = GM_OSD_FONT_SMOOTH_LEVEL_WEAK;
    osd_font2.marquee.mode = GM_OSD_MARQUEE_MODE_NONE;
    osd_font2.border.enabled = 1;
    osd_font2.border.width = 1;
    osd_font2.border.type = GM_OSD_BORDER_TYPE_WIN;
    osd_font2.border.palette_idx = 3;
    osd_font2.font_zoom = GM_OSD_FONT_ZOOM_NONE;
    gm_set_osd_font2(capture_object, &osd_font2);//set OSD attribute(设置OSD属性)
    
    if (osd_font2.font_index != NULL)
        free(osd_font2.font_index);
}

void update_osd_font_with_gm_chinese_title(void)
{
    int i, j;
    gm_osd_font_update_t new_font;
    gm_osd_font_t osd_font;

    for (j = 0; j <= 0xf; j++) {
        new_font.font_idx = osd_gm_title[j].font;
        for (i = 0; i < GM_OSD_FONT_MAX_ROW; i++) {
            new_font.bitmap[i] = (osd_gm_title[j].fbitmap[i * 2 + 1] << 8) |
                                  osd_gm_title[j].fbitmap[i * 2];
        }
        gm_update_new_font(&new_font);
    }
    
    /** setup OSD font */   
    osd_font.enabled = 1;
    osd_font.win_idx = 0;
    osd_font.align_type = GM_OSD_ALIGN_TOP_LEFT;
    osd_font.x = 0;
    osd_font.y = 0;
    osd_font.h_words = 8;  ///< The horizontal number of words of OSD window
    osd_font.v_words = 2;   ///< The vertical number of words of OSD window
    osd_font.h_space = 0;   ///< The vertical space between charater and charater
    osd_font.v_space = 0;   ///< The horizontal space between charater and charater

    osd_font.font_index_len = 16;
    osd_font.font_index[0] = (unsigned short)(FONT_INDEX_STR + 0);
    osd_font.font_index[1] = (unsigned short)(FONT_INDEX_STR + 1);
    osd_font.font_index[2] = (unsigned short)(FONT_INDEX_STR + 2);
    osd_font.font_index[3] = (unsigned short)(FONT_INDEX_STR + 3);
    osd_font.font_index[4] = (unsigned short)(FONT_INDEX_STR + 4);
    osd_font.font_index[5] = (unsigned short)(FONT_INDEX_STR + 5);
    osd_font.font_index[6] = (unsigned short)(FONT_INDEX_STR + 6);
    osd_font.font_index[7] = (unsigned short)(FONT_INDEX_STR + 7);
    osd_font.font_index[8] = (unsigned short)(FONT_INDEX_STR + 8);
    osd_font.font_index[9] = (unsigned short)(FONT_INDEX_STR + 9);
    osd_font.font_index[10] = (unsigned short)(FONT_INDEX_STR + 10);
    osd_font.font_index[11] = (unsigned short)(FONT_INDEX_STR + 11);
    osd_font.font_index[12] = (unsigned short)(FONT_INDEX_STR + 12);
    osd_font.font_index[13] = (unsigned short)(FONT_INDEX_STR + 13);
    osd_font.font_index[14] = (unsigned short)(FONT_INDEX_STR + 14);
    osd_font.font_index[15] = (unsigned short)(FONT_INDEX_STR + 15);
    
    osd_font.font_alpha = GM_OSD_FONT_ALPHA_75;
    osd_font.win_alpha = GM_OSD_FONT_ALPHA_75;
    osd_font.win_palette_idx = 0;
    osd_font.font_palette_idx = 1;
    osd_font.priority = GM_OSD_PRIORITY_MARK_ON_OSD;
    osd_font.smooth.enabled = 1;
    osd_font.smooth.level = GM_OSD_FONT_SMOOTH_LEVEL_WEAK;
    osd_font.marquee.mode = GM_OSD_MARQUEE_MODE_NONE;
    osd_font.border.enabled = 1;
    osd_font.border.width = 1;
    osd_font.border.type = GM_OSD_BORDER_TYPE_WIN;
    osd_font.border.palette_idx = 3;
    osd_font.font_zoom = GM_OSD_FONT_ZOOM_NONE;
    gm_set_osd_font(capture_object, &osd_font);//set OSD attribute(设置OSD属性)
}

void update_osd_mask(void)
{
    gm_osd_mask_t osd_mask;

    /** setup OSD mask */
    osd_mask.enabled = 1;
    osd_mask.mask_idx = 0;
    osd_mask.x = 50;
    osd_mask.y = 50;
    osd_mask.width = 80;
    osd_mask.height = 40;
    osd_mask.alpha = 5;
    osd_mask.palette_idx = 2;
    osd_mask.border.type = GM_OSD_MASK_BORDER_TYPE_TRUE;
    osd_mask.border.width = 1;
    osd_mask.align_type = GM_OSD_ALIGN_TOP_LEFT;
    gm_set_osd_mask(capture_object, &osd_mask, GM_ALL_PATH);//set OSD attribute(设置OSD属性)    
}


void update_osd_mark(void)
{
    int mark_idx;
    int ret;
    gm_osd_mark_img_table_t osd_mark_img;
    gm_osd_mark_t osd_mark;
    FILE *logo_file;
    static char *logo_img_buf;

    logo_img_buf = (char *)malloc((LOGO_WIDTH * LOGO_HEIGHT * 2));
    if (logo_img_buf == NULL) {
        perror("Error to allocate logo buf.\n");
        return;
    }

    logo_file = fopen(LOGO_YUV422_PATTERN, "rb");
    if (!logo_file) {
        printf("Error to open logo file %s!", LOGO_YUV422_PATTERN);
        free(logo_img_buf);
        return;
    }
    
    fread(logo_img_buf, (LOGO_WIDTH * LOGO_HEIGHT * 2), 1, logo_file);

    memset(&osd_mark_img, 0, sizeof(gm_osd_mark_img_table_t));
    mark_idx = 0; //idx value from 0 ~ GM_MAX_OSD_MARK_IMG_NUM
    osd_mark_img.mark_img[mark_idx].mark_exist = 1;
    osd_mark_img.mark_img[mark_idx].mark_yuv_buf = (char *)logo_img_buf;
    osd_mark_img.mark_img[mark_idx].mark_width = GM_OSD_MARK_DIM_32;
    osd_mark_img.mark_img[mark_idx].mark_height = GM_OSD_MARK_DIM_32;
    osd_mark_img.mark_img[mark_idx].mark_yuv_buf_len = (LOGO_WIDTH * LOGO_HEIGHT * 2);
    
    ret = gm_set_osd_mark_image(&osd_mark_img);
    if (ret < 0) {
        printf("Error to set OSD mark image.\n");
        free(logo_img_buf);
        return;
    }

    osd_mark.enabled = 1;
    osd_mark.mark_idx = mark_idx;
    osd_mark.x = 100;
    osd_mark.y = 100;
    osd_mark.alpha = GM_OSD_MARK_ALPHA_75;
    osd_mark.zoom = GM_OSD_MARK_ZOOM_2X;
    osd_mark.align_type = GM_OSD_ALIGN_TOP_LEFT;
    gm_set_osd_mark(capture_object, &osd_mark);//set OSD attribute(设置OSD属性)
    if (logo_img_buf)
        free(logo_img_buf);

    fclose(logo_file);
}

void update_osg_mark(void)
{
    int mark_idx;
    int ret;
    gm_osd_mark_img_table_t osd_mark_img;
    gm_osd_mark_t osd_mark;
    FILE *logo_file;
    static char *logo_img_buf;

    logo_img_buf = (char *)malloc((OSG_LOGO_WIDTH * OSG_LOGO_HEIGHT * 2));
    if (logo_img_buf == NULL) {
        perror("Error to allocate logo buf.\n");
        return;
    }

    logo_file = fopen(OSG_LOGO_RGB1555_PATTERN, "rb");
    if (!logo_file) {
        printf("Error to open logo file %s!", OSG_LOGO_RGB1555_PATTERN);
        free(logo_img_buf);
        return;
    }
    
    fread(logo_img_buf, (OSG_LOGO_WIDTH * OSG_LOGO_HEIGHT * 2), 1, logo_file);

    memset(&osd_mark_img, 0, sizeof(gm_osd_mark_img_table_t));
    mark_idx = 4; //idx value from 0 ~ GM_MAX_OSD_MARK_IMG_NUM
    osd_mark_img.mark_img[0].mark_exist = 1;
    osd_mark_img.mark_img[0].mark_yuv_buf = (char *)logo_img_buf;
    osd_mark_img.mark_img[0].mark_width = OSG_LOGO_WIDTH;
    osd_mark_img.mark_img[0].mark_height = OSG_LOGO_HEIGHT;
    osd_mark_img.mark_img[0].mark_yuv_buf_len = (OSG_LOGO_WIDTH * OSG_LOGO_HEIGHT * 2);
    osd_mark_img.mark_img[0].osg_mark_idx = 4;

    
    ret = gm_set_osd_mark_image(&osd_mark_img);
    if (ret < 0) {
        printf("Error to set OSD mark image.\n");
        free(logo_img_buf);
        return;
    }

    osd_mark.enabled = 1;
    osd_mark.mark_idx = mark_idx;
    osd_mark.x = 0;
    osd_mark.y = 0;
    osd_mark.alpha = GM_OSD_MARK_ALPHA_75;
    osd_mark.zoom = GM_OSD_MARK_ZOOM_1X;
    osd_mark.align_type = GM_OSD_ALIGN_TOP_LEFT;
    osd_mark.osg_mark_idx = 4;
    gm_set_osd_mark(capture_object, &osd_mark);//set OSD attribute(设置OSD属性)
    if (logo_img_buf)
        free(logo_img_buf);

    fclose(logo_file);
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

    gm_init(); //gmlib initial(GM库初始化)
    gm_get_sysinfo(&gm_system);

    groupfd = gm_new_groupfd(); // create new record group fd (获取groupfd)

    capture_object = gm_new_obj(GM_CAP_OBJECT); // new capture object(获取捕捉对象)
    encode_object = gm_new_obj(GM_ENCODER_OBJECT);  // // create encoder object (获取编码对象)

    ch = 0; // use capture virtual channel 0
    cap_attr.cap_vch = ch;

    //GM8210 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can scaling)
    //GM8139/GM8287 capture path 0(liveview), 1(CVBS), 2(can scaling), 3(can't scaling down)
    cap_attr.path = 3;
    cap_attr.enable_mv_data = 0;
    gm_set_attr(capture_object, &cap_attr); // set capture attribute (设置捕捉属性)

    h264e_attr.dim.width = gm_system.cap[ch].dim.width;
    h264e_attr.dim.height = gm_system.cap[ch].dim.height;
    h264e_attr.frame_info.framerate = gm_system.cap[ch].framerate;
    h264e_attr.ratectl.mode = GM_CBR;
    h264e_attr.ratectl.gop = 60;
    h264e_attr.ratectl.bitrate = 2048;  // 2Mbps
    h264e_attr.b_frame_num = 0;  // B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data = 0;  // disable H.264 motion data output
    gm_set_attr(encode_object, &h264e_attr);

    // bind channel recording (绑定捕捉对象到编码对象)
    bindfd = gm_bind(groupfd, capture_object, encode_object);
    if (gm_apply(groupfd) < 0) { // active setting (使生效)
        perror("Error! gm_apply fail, AP procedure something wrong!");
        exit(0);
    }

    ret = pthread_create(&enc_thread_id, NULL, encode_thread, (void *)ch);
    if (ret < 0) {
        perror("create encode thread failed");
        return -1;
    }

    //osd palette config and load logo mark
    ret = gm_set_palette_table(&palette);
    if (ret < 0) {
        perror("Set osd palette failed");
        return -1;
    }

    printf("Start Encoding....\n");
    printf("Please select OSD feature:\n"
           "   'f': enable OSD font\n"
           "   '2': enable OSD font2\n"
           "   'm': enable OSD Mask\n"
           "   'r': enable OSD Mark\n"
           "   'g': enable OSG Mark\n"
           "   'u': update GM chinese title\n"
           "   'q': quit\n");

    while (1) {
        key = getchar();
        if (key == 'f') {
            printf("Update OSD font...\n");
            update_osd_font();
        } else if (key == '2') {
            printf("Update OSD font2...\n");
            update_osd_font2();
        } else if (key == 'm') {
            printf("Mask enabled...\n");
            update_osd_mask();            
        } else if (key == 'r') {
            printf("Mark enabled...\n");
            update_osd_mark();
        } else if (key == 'g') {
            printf("Osg Mark enabled...\n");
            update_osg_mark();
        } else if (key == 'u') {
            printf("Update GM chinese title...\n");
            update_osd_font_with_gm_chinese_title();
        } else if (key == 'q') {
            enc_exit = 1;
            break;
        }
    }

    pthread_join(enc_thread_id, NULL);
    gm_unbind(bindfd);
    gm_apply(groupfd);
    gm_delete_obj(encode_object);
    gm_delete_obj(capture_object);
    gm_delete_groupfd(groupfd);
    gm_release();
    return ret;
}
