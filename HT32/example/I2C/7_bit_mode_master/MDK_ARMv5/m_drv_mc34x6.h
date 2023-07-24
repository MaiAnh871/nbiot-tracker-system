/*****************************************************************************
 *
 * Copyright (c) 2018 mCube, Inc.  All rights reserved.
 *
 * This source is subject to the mCube Software License.
 * This software is protected by Copyright and the information and source code
 * contained herein is confidential. The software including the source code
 * may not be copied and the information contained herein may not be used or
 * disclosed except with the written permission of mCube Inc.
 *
 * All other rights reserved.
 *****************************************************************************/

/**
 * @file    m_drv_mc34x6.h
 * @author  mCube
 * @date    20 June 2018
 * @brief   Driver interface for accelerometer mc34x6 series.
 * @see     http://www.mcubemems.com
 */

#ifndef _M_DRV_MC34X6_H_
    #define _M_DRV_MC34X6_H_

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
/** mCube functions include */
#include "m_drv_interface.h"
#include "m_drv_mc_utility.h"

/*******************************************************************************
 *** CONSTANT / DEFINE
 *******************************************************************************/
/** Ambiq SDK using uint32_t for communication. Nordic SDK using uint8_t. */
#define uint_dev    uint8_t

#define m_drv_buffer(A)                                                       \
    union                                                                     \
    {                                                                         \
        uint32_t words[(A + 3) >> 2];                                         \
        uint8_t  bytes[A];                                                    \
    }

#define M_DRV_MC34X6_RETCODE_SUCCESS                 (0)
#define M_DRV_MC34X6_RETCODE_ERROR_BUS               (-1)
#define M_DRV_MC34X6_RETCODE_ERROR_NULL_POINTER      (-2)
#define M_DRV_MC34X6_RETCODE_ERROR_STATUS            (-3)
#define M_DRV_MC34X6_RETCODE_ERROR_SETUP             (-4)
#define M_DRV_MC34X6_RETCODE_ERROR_GET_DATA          (-5)
#define M_DRV_MC34X6_RETCODE_ERROR_IDENTIFICATION    (-6)
#define M_DRV_MC34X6_RETCODE_ERROR_NO_DATA           (-7)
#define M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT    (-8)

#define M_DRV_MC34X6_AXIS_X      0
#define M_DRV_MC34X6_AXIS_Y      1
#define M_DRV_MC34X6_AXIS_Z      2
#define M_DRV_MC34X6_AXES_NUM    3

#define M_DRV_MC34X6_REG_MAP_SIZE    75

#define M_DRV_MC34X6_INTR_C_IPP_MODE_OPEN_DRAIN     (0x00)
#define M_DRV_MC34X6_INTR_C_IPP_MODE_PUSH_PULL      (0x40)

#define M_DRV_MC34X6_INTR_C_IAH_ACTIVE_LOW          (0x00)
#define M_DRV_MC34X6_INTR_C_IAH_ACTIVE_HIGH         (0x80)

#define M_DRV_MC34X6_NULL_ADDR    (0)

/*****************************************************************************
 *** DATA TYPE / STRUCTURE DEFINITION / ENUM
 *****************************************************************************/
/* Available mode in MC34X6 sensor */
typedef enum
{
    E_M_DRV_MC34X6_MODE_SLEEP = 0x00,
    E_M_DRV_MC34X6_MODE_WAKE,
    E_M_DRV_MC34X6_MODE_RESERVED,        //Reserve, not use
    E_M_DRV_MC34X6_MODE_STANDBY,
    E_M_DRV_MC34X6_MODE_END,

    E_M_DRV_MC34X6_INTR_IPP_OPEN_DRAIN_IAH_LOW      =   0,       //External pullup to VDD for INT and active low
    E_M_DRV_MC34X6_INTR_IPP_MODE_PUSH_PULL_IAH_LOW  =   0x40,    //Logic high drive level is VDD for INT and active low
    E_M_DRV_MC34X6_INTR_IPP_OPEN_DRAIN_IAH_HIGH     =   0x80,    //External pullup to VDD for INT and active high
    E_M_DRV_MC34X6_INTR_IPP_MODE_PUSH_PULL_IAH_HIGH =   0xC0,    //Logic high drive level is VDD for INT and active high
    E_M_DRV_MC34X6_INTR_END,
}   e_m_drv_mc34x6_mode_t;

/* LPF BW */
typedef enum
{
    E_M_DRV_MC34X6_LPF_DISABLE =     0x00,    //LPF DISABLE
    E_M_DRV_MC34X6_LPF_128Hz   =     0x01,    //BW1 ~ 128Hz@2048Hz ODR
    E_M_DRV_MC34X6_LPF_16Hz    =     0x04,    //BW4 ~  16Hz@2048Hz ODR
    E_M_DRV_MC34X6_LPF_ENABLE  =     0x08,    //LPF ENABLE
    E_M_DRV_MC34X6_LPF_END,
}   e_m_drv_mc34x6_lpf_t;

/* Sensor Range */
typedef enum
{
    E_M_DRV_MC34X6_RANGE_2G = 0,
    E_M_DRV_MC34X6_RANGE_4G,
    E_M_DRV_MC34X6_RANGE_8G,
    E_M_DRV_MC34X6_RANGE_16G,
    E_M_DRV_MC34X6_RANGE_12G,
    E_M_DRV_MC34X6_RANGE_24G,
    E_M_DRV_MC34X6_RANGE_END
}   e_m_drv_mc34x6_range_t;

/* Sensor Resolution */
typedef enum
{
    E_M_DRV_MC34X6_RESOLUTION_16BIT = 0,
    E_M_DRV_MC34X6_RESOLUTION_8BIT,
    E_M_DRV_MC34X6_RESOLUTION_END,
}   e_m_drv_mc34x6_resolution_t;

/* Sensor INT configuration */
typedef enum
{
    E_M_DRV_MC34X6_INT_DIS            = 0,
    E_M_DRV_MC34X6_INT_TILT_DIS       = 0,
    E_M_DRV_MC34X6_INT_TILT_EN        = 0x01,
    E_M_DRV_MC34X6_INT_FLIP_DIS       = 0,
    E_M_DRV_MC34X6_INT_FLIP_EN        = 0x02,
    E_M_DRV_MC34X6_INT_ANYM_DIS       = 0,
    E_M_DRV_MC34X6_INT_ANYM_EN        = 0x04,
    E_M_DRV_MC34X6_INT_SHAKE_DIS      = 0,
    E_M_DRV_MC34X6_INT_SHAKE_EN       = 0x08,
    E_M_DRV_MC34X6_INT_TILT35_DIS     = 0,
    E_M_DRV_MC34X6_INT_TILT35_EN      = 0x10,
    E_M_DRV_MC34X6_INT_AUTOCLR_DIS    = 0,
    E_M_DRV_MC34X6_INT_AUTOCLR_EN     = 0x40,
    E_M_DRV_MC34X6_INT_ACQ_DIS        = 0,
    E_M_DRV_MC34X6_INT_ACQ_EN         = 0x80,
    E_M_DRV_MC34X6_INT_END,
}   e_m_drv_mc34x6_int_t;

/* Tilt35 timer configuration */
typedef enum
{
    E_M_DRV_MC34X6_TILT35_1p6s         = 0,
    E_M_DRV_MC34X6_TILT35_1p8s,
    E_M_DRV_MC34X6_TILT35_2p0s,
    E_M_DRV_MC34X6_TILT35_2p2s,
    E_M_DRV_MC34X6_TILT35_2p4s,
    E_M_DRV_MC34X6_TILT35_2p6s,
    E_M_DRV_MC34X6_TILT35_2p8s,
    E_M_DRV_MC34X6_TILT35_3p0s,
    E_M_DRV_MC34X6_TILT35_END,
}   e_m_drv_mc34x6_tilt35_t;

/* Sensor Motion configuration */
typedef enum
{
    E_M_DRV_MC34X6_MCTL_DIS                  = 0,
    E_M_DRV_MC34X6_MCTL_TF_DIS               = 0,
    E_M_DRV_MC34X6_MCTL_TF_EN                = 0x01,
    E_M_DRV_MC34X6_MCTL_MLATCH_DIS           = 0,
    E_M_DRV_MC34X6_MCTL_MLATCH_EN            = 0x02,
    E_M_DRV_MC34X6_MCTL_ANYM_DIS             = 0,
    E_M_DRV_MC34X6_MCTL_ANYM_EN              = 0x04,
    E_M_DRV_MC34X6_MCTL_SHAKE_DIS            = 0,
    E_M_DRV_MC34X6_MCTL_SHAKE_EN             = 0x08,
    E_M_DRV_MC34X6_MCTL_TILT35_DIS           = 0,
    E_M_DRV_MC34X6_MCTL_TILT35_EN            = 0x10,
    E_M_DRV_MC34X6_MCTL_ZORT_POS             = 0,
    E_M_DRV_MC34X6_MCTL_ZORT_NEG             = 0x20,
    E_M_DRV_MC34X6_MCTL_RAW_PROC_STATUS_DIS  = 0,
    E_M_DRV_MC34X6_MCTL_RAW_PROC_STATUS_EN   = 0x40,
    E_M_DRV_MC34X6_MCTL_MOTION_RESET_DIS     = 0,
    E_M_DRV_MC34X6_MCTL_MOTION_RESET_EN      = 0x80,
    E_M_DRV_MC34X6_MCTL_END,
}   e_m_drv_mc34x6_mctl_t;

/* Sensor Sample Rate configuration */
typedef enum
{
    E_M_DRV_MC34X6_SR_128 =         0,
    E_M_DRV_MC34X6_SR_256            ,
    E_M_DRV_MC34X6_SR_512            ,
    RESERVE_011                      ,
    RESERVE_100                      ,
    E_M_DRV_MC34X6_SR_MAX            ,
    E_M_DRV_MC34X6_SR_END,
}   e_m_drv_mc34x6_sr_t;

/* Available registers in MC34X6 sensor device */
typedef enum
{
    E_M_DRV_MC34X6_REG_XOUT             = (0x00),   /* (r)  8-bit XOUT Accelerometer Register, signed 2??s complement format, range in +- 2g */
    E_M_DRV_MC34X6_REG_YOUT                     ,   /* (r)  8-bit YOUT Accelerometer Register, signed 2??s complement format, range in +- 2g */
    E_M_DRV_MC34X6_REG_ZOUT                     ,   /* (r)  8-bit ZOUT Accelerometer Register, signed 2??s complement format, range in +- 2g */
    E_M_DRV_MC34X6_REG_STATUS1                  ,   /* (r)  8-bit Status Register */
    E_M_DRV_MC34X6_REG_INTR_STAT_1              ,   /* (r)  8-bit Interrupt Status Register */
    E_M_DRV_MC34X6_REG_DEV_STAT                 ,   /* (r)  Device Status Register */
    E_M_DRV_MC34X6_REG_INTR_CTRL                ,   /* (rw) Interrupt Control Register */
    E_M_DRV_MC34X6_REG_MODE                     ,   /* (rw) Mode Control Register */
    E_M_DRV_MC34X6_REG_SR                       ,   /* (r)  Sample Rate Register */
    E_M_DRV_MC34X6_REG_MOTION_CTRL              ,   /* (rw) Motion Control Register */

    E_M_DRV_MC34X6_REG_XOUT_EX_L        = (0x0D),   /* (r)  16-bit XOUT Accelerometer Register */
    E_M_DRV_MC34X6_REG_XOUT_EX_H                ,
    E_M_DRV_MC34X6_REG_YOUT_EX_L                ,   /* (r)  16-bit YOUT Accelerometer Register */
    E_M_DRV_MC34X6_REG_YOUT_EX_H                ,
    E_M_DRV_MC34X6_REG_ZOUT_EX_L                ,   /* (r)  16-bit ZOUT Accelerometer Register */
    E_M_DRV_MC34X6_REG_ZOUT_EX_H                ,
    E_M_DRV_MC34X6_REG_STATUS2                  ,   /* (r)  16-bit Status Register */
    E_M_DRV_MC34X6_REG_INTR_STAT_2      = (0x14),   /* (r)  16-bit Interrupt Status Register */
    E_M_DRV_MC34X6_REG_SDM_X                    ,   /* (rw) SDM_X Control Register */
    E_M_DRV_MC34X6_REG_SDM_Y                    ,   /* (rw) SDM_Y Control Register */
    E_M_DRV_MC34X6_REG_SDM_Z                    ,   /* (rw) SDM_Z Control Register */

    E_M_DRV_MC34X6_REG_RANGE            = (0x20),   /* (rw) Range Select Control Register */
    E_M_DRV_MC34X6_REG_XOFFLE                   ,   /* (rw) X-Offset LSB Register */
    E_M_DRV_MC34X6_REG_XOFFLH                   ,   /* (rw) X-Offset MSB/XGAIN MSB Register */
    E_M_DRV_MC34X6_REG_YOFFLE                   ,   /* (rw) Y-Offset LSB Register */
    E_M_DRV_MC34X6_REG_YOFFLH                   ,   /* (rw) Y-Offset MSB/XGAIN MSB Register */
    E_M_DRV_MC34X6_REG_ZOFFLE                   ,   /* (rw) Z-Offset LSB Register */
    E_M_DRV_MC34X6_REG_ZOFFLH                   ,   /* (rw) Z-Offset MSB/XGAIN MSB Register */
    E_M_DRV_MC34X6_REG_XGAIN                    ,   /* (rw) XGAIN Register */
    E_M_DRV_MC34X6_REG_YGAIN                    ,   /* (rw) YGAIN Register */
    E_M_DRV_MC34X6_REG_ZGAIN                    ,   /* (rw) ZGAIN Register */
    E_M_DRV_MC34X6_REG_FREQ_POL                 ,   /* (rw) Polarity,Frequency Register */
    E_M_DRV_MC34X6_REG_ANA_GAIN                 ,   /* (rw) Analog Gain Register */
    E_M_DRV_MC34X6_REG_MISC                     ,   /* (rw) MISC Blocks Control Register */

    E_M_DRV_MC34X6_REG_TF_THRESH_L      = (0x40),   /* (rw) Tilt,Flip Threshold LSB Register */
    E_M_DRV_MC34X6_REG_TF_THRESH_H              ,   /* (rw) Tilt,Flip Threshold MSB Register */
    E_M_DRV_MC34X6_REG_TF_DB                    ,   /* (rw) Tilt,Flip Debounce Register */
    E_M_DRV_MC34X6_REG_AM_THRESH_L              ,   /* (rw) AnyMotion Threshold LSB Register */
    E_M_DRV_MC34X6_REG_AM_THRESH_H              ,   /* (rw) AnyMotion Threshold MSB Register */
    E_M_DRV_MC34X6_REG_AM_DB                    ,   /* (rw) AnyMotion Debounce Register */
    E_M_DRV_MC34X6_REG_SHK_THRESH_L             ,   /* (rw) Shake Threshold LSB Register */
    E_M_DRV_MC34X6_REG_SHK_THRESH_H             ,   /* (rw) Shake Threshold MSB Register */
    E_M_DRV_MC34X6_REG_PK_P2P_DUR_THRESH_L      ,   /* (rw) Peak P2P Duration Threshold LSB Register */
    E_M_DRV_MC34X6_REG_PK_P2P_DUR_THRESH_H      ,   /* (rw) Peak P2P Duration Threshold MSB Register */
    E_M_DRV_MC34X6_REG_TIMER_CTRL               ,   /* (rw) Timer Control Register */
    E_M_DRV_MC34X6_REG_MC34X6_END               ,   /* Register # < 75 */
} e_m_drv_mc34x6_reg_t;

typedef struct
{
    unsigned char    bACQ_INT;
    unsigned char    bTILT35_INT;
    unsigned char    bSHAKE_INT;
    unsigned char    bANYM_INT;
    unsigned char    bFLIP_INT;
    unsigned char    bTILT_INT;
}   s_m_drv_mc34x6_int_t;

/*******************************************************************************
 *** EXTERNAL FUNCTION
 *******************************************************************************/
extern int M_DRV_MC34X6_Init(void);
extern int M_DRV_MC34X6_SetMode(e_m_drv_mc34x6_mode_t eNextMode);
extern int M_DRV_MC34X6_ConfigRegResRngLPFCtrl(e_m_drv_mc34x6_resolution_t eCfgRes,e_m_drv_mc34x6_range_t eCfgRange, e_m_drv_mc34x6_lpf_t eCfgLPF);
extern int M_DRV_MC34X6_SetSampleRate(e_m_drv_mc34x6_sr_t eSR);
extern int M_DRV_MC34X6_ConfigINT(e_m_drv_mc34x6_int_t bINTmode);
extern int M_DRV_MC34X6_ConfigMotionCtl(e_m_drv_mc34x6_mctl_t bMotionCtl);
extern int M_DRV_MC34X6_SetTFThreshold(void);
extern int M_DRV_MC34X6_SetTFDebounce(void);
extern int M_DRV_MC34X6_SetTILT35Threshold(void);
extern int M_DRV_MC34X6_SetTILT35Timer(void);
extern int M_DRV_MC34X6_SetANYMThreshold(void);
extern int M_DRV_MC34X6_SetANYMDebounce(void);
extern int M_DRV_MC34X6_SetShakeThreshold(void);
extern int M_DRV_MC34X6_SetShake_P2P_DUR_THRESH(void);
extern int M_DRV_MC34X6_ReadData(float faOutput[M_DRV_MC34X6_AXES_NUM]);
extern int M_DRV_MC34X6_ReadRawData(short Output[M_DRV_MC34X6_AXES_NUM]);
extern int M_DRV_MC34X6_GetMotion(unsigned char *MotionStatus);
extern int M_DRV_MC34X6_HandleINT(s_m_drv_mc34x6_int_t *ptINT_Event);
extern int M_DRV_MC34X6_ReadRegMap(unsigned char baRegMap[M_DRV_MC34X6_REG_MAP_SIZE]);
extern unsigned long  M_DRV_MC36XX_GetVersion(void);

#endif    // END of _M_DRV_MC3XXX_H_
