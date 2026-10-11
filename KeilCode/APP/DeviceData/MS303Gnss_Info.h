/**
 * @file MS303Gnss_Info.h
 * @brief MS303 组合导航系统输出通讯协议（表3），帧长 164 字节
 * @date 2026-10-06
 *
 * 帧头 55H AAH，数据长度 A0H，校验为「数据长度 + 数据区」字节累加和的低 8 位。
 * 多字节按小端存放，物理量 = 原始值 * 对应当量。
 */

#ifndef __MS303GNSS_INFO_H
#define __MS303GNSS_INFO_H

#include "stdint.h"

#define MS303_GNSS_FRAME_HEADER1          (0x55U)
#define MS303_GNSS_FRAME_HEADER2          (0xAAU)
#define MS303_GNSS_FRAME_DATA_LEN         (0xA0U) /* 160：offset 3~162 */
#define MS303_GNSS_FRAME_SIZE             (164U)

/* 当量 */
#define MS303_GNSS_SCALE_GYRO             (0.0000005f) /* deg/s */
#define MS303_GNSS_SCALE_ACC              (0.000001f)  /* m/s^2 */
#define MS303_GNSS_SCALE_LON_LAT          (0.0000001f) /* deg */
#define MS303_GNSS_SCALE_ALT              (0.00001f)   /* m */
#define MS303_GNSS_SCALE_VEL              (0.005f)     /* m/s */
#define MS303_GNSS_SCALE_ATT              (0.006f)     /* deg */

/* 系统状态字 */
#define MS303_SYS_SELF_ALIGNING           (0x10U)
#define MS303_SYS_SELF_ALIGN_OK           (0x12U)
#define MS303_SYS_SELF_ALIGN_ERR          (0x1FU)
#define MS303_SYS_COARSE_ALIGN            (0x33U)
#define MS303_SYS_FINE_ALIGN              (0x35U)
#define MS303_SYS_PURE_INS                (0x41U)
#define MS303_SYS_ZERO_SPEED_CORR         (0x43U)
#define MS303_SYS_VEL_POS                 (0x44U)
#define MS303_SYS_VEL_POS_HDG             (0x45U)
#define MS303_SYS_VEHICLE_CONSTRAINED     (0x46U)
#define MS303_SYS_ODO_COMBINED            (0x47U)
#define MS303_SYS_CALIB_IN_PROGRESS_1     (0x51U)
#define MS303_SYS_CALIB_IN_PROGRESS_2     (0x53U)
#define MS303_SYS_CALIB_IN_PROGRESS_3     (0x55U)
#define MS303_SYS_CALIB_VALID             (0x56U)
#define MS303_SYS_CALIB_FLASH_OK          (0x57U)
#define MS303_SYS_CALIB_ERR               (0x5FU)

/* 定位/定向有效性：bit0-3 定位，bit4-7 定向 */
#define MS303_FIX_NONE                    (0x0U)
#define MS303_FIX_SINGLE                  (0x1U)
#define MS303_FIX_FLOAT                   (0x2U)
#define MS303_FIX_FIXED                   (0x3U)
#define MS303_POS_FIX(v)                  ((uint8_t)((v) & 0x0FU))
#define MS303_HDG_FIX(v)                  ((uint8_t)(((v) >> 4) & 0x0FU))

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#pragma pack(push, 1)
#endif

typedef struct
{
    uint8_t  header1;              /* 0  55H */
    uint8_t  header2;              /* 1  AAH */
    uint8_t  data_len;             /* 2  A0H */
    uint32_t pc_num;               /* 3  中断计数 */
    uint8_t  sys_status;           /* 7  系统状态字 */
    uint8_t  realtime_status;      /* 8  实时状态字 */
    uint8_t  reserved_9[2];        /* 9 */
    uint8_t  reserved_11[4];       /* 11 */
    int32_t  gyro_x;               /* 15 X陀螺  * 0.0000005 deg/s */
    int32_t  gyro_y;               /* 19 Y陀螺 */
    int32_t  gyro_z;               /* 23 Z陀螺 */
    int32_t  acc_x;                /* 27 X加表  * 0.000001 m/s^2 */
    int32_t  acc_y;                /* 31 Y加表 */
    int32_t  acc_z;                /* 35 Z加表 */
    int32_t  fused_lon;            /* 39 组合经度 * 0.0000001 deg */
    int32_t  fused_lat;            /* 43 组合纬度 * 0.0000001 deg */
    int32_t  fused_alt;            /* 47 组合高度 * 0.00001 m */
    int16_t  fused_vn;             /* 51 组合北速 * 0.005 m/s */
    int16_t  fused_vu;             /* 53 组合天速 */
    int16_t  fused_ve;             /* 55 组合东速 */
    int16_t  roll;                 /* 57 滚动角 * 0.006 deg */
    int16_t  heading;              /* 59 航向角 * 0.006 deg，北偏西为正 */
    int16_t  pitch;                /* 61 俯仰角 * 0.006 deg */
    int32_t  sat_lon;              /* 63 卫星经度 * 0.0000001 deg */
    int32_t  sat_lat;              /* 67 卫星纬度 * 0.0000001 deg */
    int32_t  sat_alt;              /* 71 卫星高度 * 0.00001 m */
    int16_t  sat_vn;               /* 75 卫星北速 * 0.005 m/s */
    int16_t  sat_vu;               /* 77 卫星天速 */
    int16_t  sat_ve;               /* 79 卫星东速 */
    int8_t   temperature;          /* 81 温度 degC */
    uint8_t  pos_hdg_valid;        /* 82 定位定向有效性 */
    uint32_t gps_tow;              /* 83 卫星周内秒 */
    uint8_t  year;                 /* 87 */
    uint8_t  month;                /* 88 */
    uint8_t  day;                  /* 89 */
    uint8_t  sat_num;              /* 90 定位星数 */
    int16_t  sat_heading;          /* 91 卫星航迹角 * 0.006 deg */
    int16_t  dual_ant_heading;     /* 93 双天线航向 * 0.006 deg */
    uint8_t  reserved_95[2];       /* 95 */
    uint8_t  reserved_97[14];      /* 97 */
    uint8_t  reserved_111[12];     /* 111 */
    int32_t  au;                   /* 123 天向加速度 m/s^2 */
    int32_t  af;                   /* 127 前向加速度 m/s^2 */
    int32_t  ar;                   /* 131 右向加速度 m/s^2 */
    int32_t  heave;                /* 135 相对升沉 m */
    int32_t  filter_out;           /* 139 滤波输出 */
    int32_t  filter_in;            /* 143 滤波输入 */
    int32_t  main_freq;            /* 147 主频率 */
    int32_t  time_int;             /* 151 数据时间整数部分 */
    int32_t  time_frac;            /* 155 数据时间小数部分 */
    uint8_t  reserved_159[4];      /* 159 */
    uint8_t  checksum;             /* 163 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
} MS303_GNSS_FRAME_t;
#pragma pack(pop)
#else
} __attribute__((packed)) MS303_GNSS_FRAME_t;
#endif

typedef struct
{
    uint8_t  id;
    uint8_t  status;
    uint8_t  error;
    MS303_GNSS_FRAME_t frame;
} MS303_GNSS_INFO_t;

#endif /* __MS303GNSS_INFO_H */
