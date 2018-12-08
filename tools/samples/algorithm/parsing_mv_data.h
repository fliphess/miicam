#ifndef _PARSING_MV_DATA_H
#define _PARSING_MV_DATA_H

#define MOTION_PARSING_ERROR -1
#define MD_S_NOT_READY        0
#define MD_S_INIT_OK          1

struct mv_cfg_t {
    unsigned int k_height;      /*  vertical resolution of kernel mv data   */
    unsigned int k_width;       /*  horizontal resolution of kernel mv data */
    unsigned int k_mb_height;   /*  Macroblock vertical resolution of kernel mv data
                                    k_width < 1024, k_mb_height = 16; otherwise,
                                    k_mb_height = 32   */
    unsigned int k_mb_width;    /*  Macroblock horizontal resolution of kernel mv data
                                    k_width < 1024, k_mb_width = 16; otherwise, k_mb_width = 32  */
    unsigned char *active_flag; /*  the parsed motion data for algorithm  */
    unsigned int active_num;    /*  the number of Macroblock with motion for algorithm  */
};

extern int parsing_mv_data(unsigned char *event_buf, struct mv_cfg_t *mv_param, unsigned int u_width,
                           unsigned int u_height, unsigned int u_mb_width, unsigned int u_mb_height, 
                           unsigned int sensitive_th, unsigned int frame_count, unsigned int mb_w_num,
                           unsigned int mb_h_num, int mv_idx);
extern int parsing_mv_initial(unsigned int mb_w_num, unsigned int mb_h_num, int mv_idx);
extern int parsing_mv_end(struct mv_cfg_t *mv_param, int mv_idx);

#endif

