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
 * @file    m_drv_mc34x6.c
 * @author  mCube
 * @date    20 June 2018
 * @brief   Driver interface for accelerometer mc34x6 series.
 * @see     http://www.mcubemems.com
 */

/******************************************************************************
 *** INFORMATION
 *****************************************************************************/
#define M_DRV_MC34X6_VERSION_AlGOTITHM_MAJOR                 0
#define M_DRV_MC34X6_VERSION_AlGOTITHM_MINOR                 0
#define M_DRV_MC34X6_VERSION_AlGOTITHM_BUILD                 0

#define M_DRV_MC34X6_VERSION_INTERFACE_MAJOR                 1
#define M_DRV_MC34X6_VERSION_INTERFACE_MINOR                 1
#define M_DRV_MC34X6_VERSION_INTERFACE_BUILD                 1

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
/* mCube function includes */
#include "m_drv_mc34x6.h"

/******************************************************************************
 *** CONFIGURATION
 *****************************************************************************/
#define M_DRV_MC36X6_CFG_I2C_ADDR    (0x4C)
//#define M_DRV_MC36X6_CFG_I2C_ADDR    (0x6C)

//#define DEBUG_MODE

#define M_DRV_MC34X6_CFG_RANGE                                              \
            E_M_DRV_MC34X6_RANGE_2G
#define M_DRV_MC34X6_CFG_RESOLUTION                                         \
            E_M_DRV_MC34X6_RESOLUTION_16BIT
#define M_DRV_MC34X6_CFG_SAMPLE_RATE                                        \
            E_M_DRV_MC34X6_SR_512
#define M_DRV_MC34X6_CFG_LPF                                                \
            E_M_DRV_MC34X6_LPF_DISABLE
#define M_DRV_MC34X6_CFG_ORIENTATION_MAP                                    \
            E_M_DRV_UTIL_ORIENTATION_TOP_RIGHT_UP
#define M_DRV_MC34X6_CFG_INT_MODE_DEFAULT                                   \
            M_DRV_MC34X6_INTR_C_IAH_ACTIVE_LOW                              \
          | M_DRV_MC34X6_INTR_C_IPP_MODE_PUSH_PULL

#define M_DRV_MC34X6_CFG_FTTHRESHOLD        200
#define M_DRV_MC34X6_CFG_FTDEBOUNCE         50
#define M_DRV_MC34X6_CFG_TILT35THRESHOLD    20
#define M_DRV_MC34X6_CFG_TILT35TIMER        E_M_DRV_MC34X6_TILT35_2p0s
#define M_DRV_MC34X6_CFG_ANYMTHRESHOLD      400            //If LPF on, recommend ANYM Thr < 150.
#define M_DRV_MC34X6_CFG_ANYMDEBOUNCE       30
#define M_DRV_MC34X6_CFG_SHAKETHRESHOLD     300
#define M_DRV_MC34X6_CFG_SHAKECOUNT         1
#define M_DRV_MC34X6_CFG_SHAKEP2PDURATION   10

#define M_DRV_MC34X6_GRAVITY                  (9.807f)
#define M_DRV_MC34X6_GAIN_16BIT                16384
#define M_DRV_MC34X6_GAIN_8BIT                64

/******************************************************************************
 *** MACRO
 *****************************************************************************/
#define _M_DRV_MC34X6_REG_WRITE(bRegAddr, pbDataBuf, bLength)               \
        mcube_write_regs(1, M_DRV_MC36X6_CFG_I2C_ADDR,                      \
                         bRegAddr, pbDataBuf, bLength)
#define _M_DRV_MC34X6_REG_READ(bRegAddr, pbDataBuf, bLength)                \
        mcube_read_regs(1, M_DRV_MC36X6_CFG_I2C_ADDR,                       \
                        bRegAddr, pbDataBuf, bLength)

#define _M_DRV_MC34X6_REG_STATUS_MODE(bRegStatus)                   (bRegStatus & 0x03)

#define _M_DRV_MC34X6_REG_INTR_STAT_2_ACQ_INT(bRegStatus2)          ((bRegStatus2 >> 7) & 0x01)
#define _M_DRV_MC34X6_REG_INTR_STAT_2_TILT35_INT(bRegStatus2)       ((bRegStatus2 >> 4) & 0x01)
#define _M_DRV_MC34X6_REG_INTR_STAT_2_SHAKE_INT(bRegStatus2)        ((bRegStatus2 >> 3) & 0x01)
#define _M_DRV_MC34X6_REG_INTR_STAT_2_ANYM_INT(bRegStatus2)         ((bRegStatus2 >> 2) & 0x01)
#define _M_DRV_MC34X6_REG_INTR_STAT_2_FLIP_INT(bRegStatus2)         ((bRegStatus2 >> 1) & 0x01)
#define _M_DRV_MC34X6_REG_INTR_STAT_2_TILT_INT(bRegStatus2)         ((bRegStatus2) & 0x01)

#ifdef DEBUG_MODE
extern void mcube_printf(const char *pui8Fmt, ...);
#define M_PRINTF(...)       do {                                            \
                            mcube_printf(__VA_ARGS__);                      \
                            } while (0);
#else
#define M_PRINTF(...)
#endif

/*******************************************************************************
 *** STATIC VARIABLE
 *******************************************************************************/
static e_m_drv_mc34x6_range_t         s_eRange       = M_DRV_MC34X6_CFG_RANGE;
static e_m_drv_mc34x6_resolution_t    s_eResolution  = M_DRV_MC34X6_CFG_RESOLUTION;
static e_m_drv_mc34x6_lpf_t           s_eLPF         = M_DRV_MC34X6_CFG_LPF;

static uint16_t       s_bCfgFTThr                    = M_DRV_MC34X6_CFG_FTTHRESHOLD;         //Flip, Tilt threshold
static uint8_t        s_bCfgFTDebounce               = M_DRV_MC34X6_CFG_FTDEBOUNCE;          //Flip, Tilt Debounce
static uint16_t       s_bCfgTILT35Thr                = M_DRV_MC34X6_CFG_TILT35THRESHOLD;     //Tilt35 threshold
static uint16_t       s_bCfgTILT35Timer              = M_DRV_MC34X6_CFG_TILT35TIMER;         //Tilt35 timer
static uint16_t       s_bCfgANYMThr                  = M_DRV_MC34X6_CFG_ANYMTHRESHOLD;       //Anymotion threshold
static uint8_t        s_bCfgANYMDebounce             = M_DRV_MC34X6_CFG_ANYMDEBOUNCE;        //Anymotion Debounce
static uint16_t       s_bCfgShakeThr                 = M_DRV_MC34X6_CFG_SHAKETHRESHOLD;      //Shake threshold
static uint8_t        s_bCfgShakeCnt                 = M_DRV_MC34X6_CFG_SHAKECOUNT;          //Shake count
static uint16_t       s_bCfgShakeP2PDuration         = M_DRV_MC34X6_CFG_SHAKEP2PDURATION;    //Shake peak to peak duration

static uint8_t        s_bCfgINT                      = 0x00;
static uint8_t        s_bMotionCtl                   = 0x00;
static uint8_t        s_bCfgRngResolLPF              = 0x00;
static uint8_t        s_bMotionDetect                = 0x00;

/******************************************************************************
 *** STATIC FUNCTION
 *****************************************************************************/
/******************************************************************************
 *** _M_DRV_MC34X6_Delay
 *****************************************************************************/
void _M_DRV_MC34X6_Delay(uint32_t dwMs) {
    mcube_delay_ms(dwMs);
}

/******************************************************************************
 *** _M_DRV_MC34X6_Reset
 *****************************************************************************/
static int    _M_DRV_MC34X6_Reset(void)
{
    uint8_t _bRegData = 0x43;
    uint8_t    _bReset[2] = {0};

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, (void *)&_bRegData, 1);

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_DEV_STAT, (void *)&_bRegData, 1);

    if (0x00 == (_bRegData & 0x40)) {
        _bRegData = 0xE1;
        _M_DRV_MC34X6_REG_WRITE(0x1B, (void *)&_bRegData, 1);

        _bRegData = 0x25;
        _M_DRV_MC34X6_REG_WRITE(0x1B, (void *)&_bRegData, 1);
    }

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_DEV_STAT, (void *)&_bRegData, 1);

    if (_bRegData & 0x40) {
        _bRegData = 0xE1;
        _M_DRV_MC34X6_REG_WRITE(0x1B, (void *)&_bRegData, 1);

        _bRegData = 0x25;
        _M_DRV_MC34X6_REG_WRITE(0x1B, (void *)&_bRegData, 1);
    }

    _bReset[0] = 0x00;
    _bReset[1] = 0x00;
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_TF_THRESH_L, (void *)&_bReset , 2);        //Reset Tilt,Flip,Tilt35 Threshold Reg LSB(0x40)
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_TF_DB, (void *)&_bReset , 1);              //Reset Tilt,Flip Debounce
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_TIMER_CTRL, (void *)&_bReset , 1);         //Reset Tilt35 Timer
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_THRESH_L, (void *)&_bReset , 2);        //Reset Anymotion Threshold Reg LSB(0x43)
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_DB, (void *)&_bReset , 1);              //Reset Anymotion Debounce
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_SHK_THRESH_L, (void *)&_bReset , 2);       //Reset hake Threshold Reg LSB(0x46)
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_PK_P2P_DUR_THRESH_L, (void *)&_bReset , 2);//Reset Shake peak to peak duration

    _bRegData = 0x41;
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, (void *)&_bRegData, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC36XX_SetMode
 *****************************************************************************/
static int    _M_DRV_MC34X6_SetMode(e_m_drv_mc34x6_mode_t eNextMode)
{
    uint8_t _bCurrMode = 0;
    uint8_t _bRegMODE_C = 0;
    uint8_t _bGuard = 0;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_DEV_STAT, &_bCurrMode, 1);

    if (eNextMode == _M_DRV_MC34X6_REG_STATUS_MODE(_bCurrMode))
        return (M_DRV_MC34X6_RETCODE_ERROR_STATUS);

    switch((int)eNextMode)
    {
    case E_M_DRV_MC34X6_MODE_SLEEP:
        M_PRINTF("[%s] MODE_SLEEP\r\n", __func__);
        break;
    case E_M_DRV_MC34X6_MODE_STANDBY:
        M_PRINTF("[%s] MODE_STANDBY\r\n", __func__);
        break;
    default:
        _bRegMODE_C |= M_DRV_MC34X6_CFG_INT_MODE_DEFAULT;
        M_PRINTF("[%s] MODE WAKE\r\n", __func__);
        break;
    }

    _bRegMODE_C |= eNextMode;

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bRegMODE_C, 1);
    M_PRINTF("[%s] REG[0x%02X] 0x%02X", __func__, E_M_DRV_MC34X6_REG_MODE, _bRegMODE_C);

    while (1)
    {
        _bGuard++;

        _M_DRV_MC34X6_Delay(10240);

        _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_DEV_STAT, &_bCurrMode, 1);

        if (eNextMode == _M_DRV_MC34X6_REG_STATUS_MODE(_bCurrMode))
            break;

        if (_bGuard > 64)
            return (M_DRV_MC34X6_RETCODE_ERROR_SETUP);
    }

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC34X6_ReadRawData
 *****************************************************************************/
static int _M_DRV_MC34X6_ReadRawData(signed short Output[M_DRV_MC34X6_AXES_NUM])
{
    M_PRINTF("[%s] \r\n", __func__);

    uint8_t    _bPrjCfgResolution            = s_eResolution ;
    signed short _waRaw[M_DRV_MC34X6_AXES_NUM] = { 0 };
    m_drv_buffer(180) _baData = {0};
    const S_M_DRV_MC_UTIL_OrientationReMap *_ptOrienMap                     \
            = &g_MDrvUtilOrientationReMap[M_DRV_MC34X6_CFG_ORIENTATION_MAP];

    if (_bPrjCfgResolution > E_M_DRV_MC34X6_RESOLUTION_16BIT)
    {
        _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_XOUT, (uint_dev *)_baData.words, 3);

        _waRaw[M_DRV_MC34X6_AXIS_X] = ((signed char) _baData.bytes[0]);
        _waRaw[M_DRV_MC34X6_AXIS_Y] = ((signed char) _baData.bytes[1]);
        _waRaw[M_DRV_MC34X6_AXIS_Z] = ((signed char) _baData.bytes[2]);
    }
    else
    {
        _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_XOUT_EX_L, (uint_dev *)_baData.words, 6);        //    XOUT data Get

        _waRaw[M_DRV_MC34X6_AXIS_X] = ((signed short) ((_baData.bytes[0])       \
                                                     | (_baData.bytes[1] << 8)));
        _waRaw[M_DRV_MC34X6_AXIS_Y] = ((signed short) ((_baData.bytes[2])       \
                                                     | (_baData.bytes[3] << 8)));
        _waRaw[M_DRV_MC34X6_AXIS_Z] = ((signed short) ((_baData.bytes[4])       \
                                                     | (_baData.bytes[5] << 8)));
    }

    Output[M_DRV_MC34X6_AXIS_X] =
            ((_ptOrienMap->bSign[M_DRV_MC34X6_AXIS_X]                       \
            * _waRaw[_ptOrienMap->bMap[M_DRV_MC34X6_AXIS_X]]));
    Output[M_DRV_MC34X6_AXIS_Y] =
            ((_ptOrienMap->bSign[M_DRV_MC34X6_AXIS_Y]                       \
            * _waRaw[_ptOrienMap->bMap[M_DRV_MC34X6_AXIS_Y]]));
    Output[M_DRV_MC34X6_AXIS_Z] =
            ((_ptOrienMap->bSign[M_DRV_MC34X6_AXIS_Z]                       \
            * _waRaw[_ptOrienMap->bMap[M_DRV_MC34X6_AXIS_Z]]));

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** FUNCTION
 *****************************************************************************/
/******************************************************************************
 *** M_DRV_MC34X6_Init
 *****************************************************************************/
int    M_DRV_MC34X6_Init(void)
{
    M_PRINTF("[%s]", __func__);
    _M_DRV_MC34X6_Reset();

    M_DRV_MC34X6_SetSampleRate((e_m_drv_mc34x6_sr_t)M_DRV_MC34X6_CFG_SAMPLE_RATE);

    M_DRV_MC34X6_ConfigINT((e_m_drv_mc34x6_int_t)
                           (E_M_DRV_MC34X6_INT_TILT_EN |                    \
                            E_M_DRV_MC34X6_INT_FLIP_EN |                    \
                            E_M_DRV_MC34X6_INT_ANYM_EN |                    \
                            E_M_DRV_MC34X6_INT_SHAKE_EN |                   \
                            E_M_DRV_MC34X6_INT_TILT35_EN |                  \
                            E_M_DRV_MC34X6_INT_AUTOCLR_EN |                 \
                            E_M_DRV_MC34X6_INT_ACQ_EN));
    M_DRV_MC34X6_ConfigINT(E_M_DRV_MC34X6_INT_DIS);

    M_DRV_MC34X6_ConfigMotionCtl((e_m_drv_mc34x6_mctl_t)
                                 (E_M_DRV_MC34X6_MCTL_TF_EN |               \
                                  E_M_DRV_MC34X6_MCTL_MLATCH_EN |           \
                                  E_M_DRV_MC34X6_MCTL_ANYM_EN |             \
                                  E_M_DRV_MC34X6_MCTL_SHAKE_EN |            \
                                  E_M_DRV_MC34X6_MCTL_TILT35_EN |           \
                                  E_M_DRV_MC34X6_MCTL_ZORT_NEG |            \
                                  E_M_DRV_MC34X6_MCTL_RAW_PROC_STATUS_EN |  \
                                  E_M_DRV_MC34X6_MCTL_MOTION_RESET_EN));
    M_DRV_MC34X6_ConfigMotionCtl(E_M_DRV_MC34X6_MCTL_DIS);

    M_DRV_MC34X6_ConfigRegResRngLPFCtrl(s_eResolution,        \
                                        s_eRange,             \
                                        s_eLPF);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC36XX_SetMode
 *****************************************************************************/
int M_DRV_MC34X6_SetMode(e_m_drv_mc34x6_mode_t eNextMode)
{
    M_PRINTF("[%s]", __func__);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);
    _M_DRV_MC34X6_SetMode(eNextMode);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_ConfigRegResRngLPFCtrl
 *****************************************************************************/
int    M_DRV_MC34X6_ConfigRegResRngLPFCtrl(e_m_drv_mc34x6_resolution_t eCfgRes,e_m_drv_mc34x6_range_t eCfgRange, e_m_drv_mc34x6_lpf_t eCfgLPF)
{
    M_PRINTF("[%s]", __func__);

    uint8_t    _bPreMode    = 0;
    uint8_t    _bLPFModeCheck    = 0;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);

    s_bCfgRngResolLPF = (((eCfgRes << 7) & 0x80) | ((eCfgRange << 4) & 0x70) | eCfgLPF);

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_RANGE, &s_bCfgRngResolLPF, 1);

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_RANGE, &_bLPFModeCheck, 1);
    if ((_bLPFModeCheck & 0x08) >> 3)        //    data Get
        M_PRINTF("0x20:%x LPF Enable.", _bLPFModeCheck);
    else
        M_PRINTF("0x20:%x LPF Disable.", _bLPFModeCheck);

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    s_eRange      = eCfgRange;
    s_eResolution = eCfgRes;
    s_eLPF           = eCfgLPF;

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** _M_DRV_MC34X6_SetSampleRate (RATE/CLK/OSR)
 *****************************************************************************/
int    M_DRV_MC34X6_SetSampleRate(e_m_drv_mc34x6_sr_t eSR)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;
    uint8_t _beSR = eSR;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_SR, &_beSR, 1);
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_ConfigINT
 *****************************************************************************/
int    M_DRV_MC34X6_ConfigINT(e_m_drv_mc34x6_int_t bINTmode)
{
    M_PRINTF("[%s]", __func__);

    uint8_t    _bPreMode = 0;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);    //STANDBY

    s_bCfgINT = bINTmode;

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_INTR_CTRL, &s_bCfgINT, 1);
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_ConfigMotionCtl
 *****************************************************************************/
int    M_DRV_MC34X6_ConfigMotionCtl(e_m_drv_mc34x6_mctl_t bMotionCtl)
{
    M_PRINTF("[%s]", __func__);

    uint8_t    _bPreMode = 0;
    uint8_t    _baData = 0;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);    //STANDBY

    s_bMotionCtl = bMotionCtl;

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MOTION_CTRL, &s_bMotionCtl, 1);
    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MOTION_CTRL, &_baData, 1);

    if (_baData > 0 )
    {
        if ((_baData & E_M_DRV_MC34X6_MCTL_TF_EN) > 0 )
            M_PRINTF("TF Mode Enable.");
        if ((_baData & E_M_DRV_MC34X6_MCTL_MLATCH_EN) > 0 )
            M_PRINTF("Motion Latch Enable.");
        if ((_baData & E_M_DRV_MC34X6_MCTL_ANYM_EN) > 0 )
            M_PRINTF("Any Motion Mode Enable.");
        if ((_baData & E_M_DRV_MC34X6_MCTL_SHAKE_EN) > 0 )
            M_PRINTF("Shake Mode Enable.");
        if ((_baData & E_M_DRV_MC34X6_MCTL_TILT35_EN) > 0 )
            M_PRINTF("Tilt 35 Mode Enable.");
    }

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_SetTFThreshold
 *****************************************************************************/
int    M_DRV_MC34X6_SetTFThreshold(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;
    uint8_t    _bFTThr[2] = {0};

    if (s_bCfgFTThr == 0)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _bFTThr[0] = (s_bCfgFTThr & 0x00ff);
    _bFTThr[1] = ((s_bCfgFTThr & 0x7f00) >> 8 );

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                //STANDBY
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_TF_THRESH_L, &_bFTThr[0] , 2);            //Tilt,Flip Threshold Reg LSB(0x40)
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);

}

/******************************************************************************
 *** M_DRV_MC34X6_SetTFDebounce
 *****************************************************************************/
int    M_DRV_MC34X6_SetTFDebounce(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;

    if (s_bCfgFTDebounce == 0)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                //STANDBY
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_TF_DB, &s_bCfgFTDebounce , 1);        //Tilt,Flip Debounce
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_SetTILT35Threshold
 *****************************************************************************/
int    M_DRV_MC34X6_SetTILT35Threshold(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;
    uint8_t    _bFTThr[2] = {0};
    uint8_t    _bANYMFTThr[2] = {0};

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_RANGE, &_bPreMode, 1);    //Check LPF Status

    if ((_bPreMode & 0x08) == E_M_DRV_MC34X6_LPF_DISABLE) {
        s_bCfgANYMThr          = 250;        //New Anymotion threshold for shake mode without LPF
        s_bCfgANYMDebounce     = 50;        //New Anymotion debounce for shake mode without LPF
    } else {
        s_bCfgANYMThr          = 16;        //New Anymotion threshold for shake mode with LPF
        s_bCfgANYMDebounce     = 100;        //New Anymotion debounce for shake mode with LPF
    }

    if (s_bCfgTILT35Thr == 0)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _bFTThr[0] = (s_bCfgTILT35Thr & 0x00ff);
    _bFTThr[1] = ((s_bCfgTILT35Thr & 0x7f00) >> 8 );
    _bANYMFTThr[0] = (s_bCfgANYMThr & 0x00ff);
    _bANYMFTThr[1] = ((s_bCfgANYMThr & 0x7f00) >> 8 );

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                //STANDBY
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_THRESH_L, &_bANYMFTThr[0] , 2);        //Anymotion Threshold Reg LSB(0x43)
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_DB, &s_bCfgANYMDebounce , 1);        //Anymotion Debounce
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_TF_THRESH_L, &_bFTThr[0] , 2);            //Tilt35 Threshold Reg LSB(0x40)

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_SetTILT35Timer
 *****************************************************************************/
int    M_DRV_MC34X6_SetTILT35Timer(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;
    uint8_t _bPreReg = 0;
    uint8_t    _bFTThr = 0;

    if (s_bCfgTILT35Timer > 7)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);
    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_TIMER_CTRL, &_bPreReg, 1);

    _bFTThr = ((_bPreReg & 0xf8) | (s_bCfgTILT35Timer & 0x07));
    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                //STANDBY
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_TIMER_CTRL, &_bFTThr , 1);            //Tilt35 Timer
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_SetANYMThreshold
 *****************************************************************************/
int    M_DRV_MC34X6_SetANYMThreshold(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;
    uint8_t    _bFTThr[2] = {0};

    s_bCfgANYMThr          = M_DRV_MC34X6_CFG_ANYMTHRESHOLD;        //Anymotion threshold

    if (s_bCfgANYMThr == 0)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _bFTThr[0] = (s_bCfgANYMThr & 0x00ff);
    _bFTThr[1] = ((s_bCfgANYMThr & 0x7f00) >> 8 );

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);
    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                //STANDBY
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_THRESH_L, &_bFTThr[0] , 2);            //Anymotion Threshold Reg LSB(0x43)
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_SetANYMDebounce
 *****************************************************************************/
int    M_DRV_MC34X6_SetANYMDebounce(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;

    s_bCfgANYMDebounce     = M_DRV_MC34X6_CFG_ANYMDEBOUNCE;        //Anymotion debounce

    if (s_bCfgANYMDebounce == 0)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);
    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                //STANDBY
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_DB, &s_bCfgANYMDebounce , 1);        //Anymotion Debounce
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_SetShakeThreshold
 *****************************************************************************/
int    M_DRV_MC34X6_SetShakeThreshold(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;
    uint8_t    _bShakeThr[2] = {0};
    uint8_t    _bFTThr[2] = {0};

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_RANGE, &_bPreMode, 1);    //Check LPF Status

    if ((_bPreMode & 0x08) == E_M_DRV_MC34X6_LPF_DISABLE) {
        s_bCfgANYMThr          = 500;        //New Anymotion threshold for shake mode without LPF
        s_bCfgANYMDebounce     = 100;        //New Anymotion debounce for shake mode without LPF
    } else {
        s_bCfgANYMThr          = 16;        //New Anymotion threshold for shake mode with LPF
        s_bCfgANYMDebounce     = 100;        //New Anymotion debounce for shake mode with LPF
    }

    if (s_bCfgShakeThr == 0)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _bShakeThr[0] = (s_bCfgShakeThr & 0x00ff);
    _bShakeThr[1] = ((s_bCfgShakeThr & 0xff00) >> 8 );
    _bFTThr[0] = (s_bCfgANYMThr & 0x00ff);
    _bFTThr[1] = ((s_bCfgANYMThr & 0x7f00) >> 8 );

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                    //STANDBY

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_THRESH_L, &_bFTThr[0] , 2);                //Anymotion Threshold Reg LSB(0x43)
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_AM_DB, &s_bCfgANYMDebounce , 1);            //Anymotion Debounce
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_SHK_THRESH_L, &_bShakeThr[0] , 2);            //Shake Threshold Reg LSB(0x46)

    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_SetShake_P2P_DUR_THRESH
 *****************************************************************************/
int    M_DRV_MC34X6_SetShake_P2P_DUR_THRESH(void)
{
    M_PRINTF("[%s]", __func__);

    uint8_t _bPreMode = 0;
    uint8_t    _bShakeP2P_DUR_THR[2] = {0};

    if (s_bCfgShakeP2PDuration == 0)
        return M_DRV_MC34X6_RETCODE_ERROR_WRONG_ARGUMENT;

    _bShakeP2P_DUR_THR[0] = (s_bCfgShakeP2PDuration & 0x00ff);
    _bShakeP2P_DUR_THR[1] = (((s_bCfgShakeP2PDuration  & 0x0f00) >> 8 ) | ((s_bCfgShakeCnt & 0x07) << 4));

    _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);
    _M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);                                                    //STANDBY
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_PK_P2P_DUR_THRESH_L, &_bShakeP2P_DUR_THR[0] , 2);           //Shake peak to peak duration
    _M_DRV_MC34X6_REG_WRITE(E_M_DRV_MC34X6_REG_MODE, &_bPreMode, 1);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_ReadData
 *****************************************************************************/
int    M_DRV_MC34X6_ReadData(float faOutput[M_DRV_MC34X6_AXES_NUM])
{
    M_PRINTF("[%s] \r\n", __func__);

    uint8_t    _bPrjCfgResolution            = s_eResolution ;
    uint8_t    _bPrjCfgRange                  = 0;
    signed short _waRaw[M_DRV_MC34X6_AXES_NUM] = { 0 };

    _M_DRV_MC34X6_ReadRawData(_waRaw);

    switch((int)s_eRange)
    {
        case E_M_DRV_MC34X6_RANGE_2G:
            _bPrjCfgRange = 1;
            break;
        case E_M_DRV_MC34X6_RANGE_4G:
            _bPrjCfgRange = 2;
            break;
        case E_M_DRV_MC34X6_RANGE_8G:
            _bPrjCfgRange = 4;
            break;
        case E_M_DRV_MC34X6_RANGE_16G:
            _bPrjCfgRange = 8;
            break;
        case E_M_DRV_MC34X6_RANGE_12G:
            _bPrjCfgRange = 6;
            break;
        case E_M_DRV_MC34X6_RANGE_24G:
            _bPrjCfgRange = 12;
            break;
    }

    switch((int)_bPrjCfgResolution)
    {
        case E_M_DRV_MC34X6_RESOLUTION_16BIT:
             faOutput[M_DRV_MC34X6_AXIS_X] = (float)((_waRaw[M_DRV_MC34X6_AXIS_X] * M_DRV_MC34X6_GRAVITY) / (M_DRV_MC34X6_GAIN_16BIT/_bPrjCfgRange));
             faOutput[M_DRV_MC34X6_AXIS_Y] = (float)((_waRaw[M_DRV_MC34X6_AXIS_Y] * M_DRV_MC34X6_GRAVITY) / (M_DRV_MC34X6_GAIN_16BIT/_bPrjCfgRange));
             faOutput[M_DRV_MC34X6_AXIS_Z] = (float)((_waRaw[M_DRV_MC34X6_AXIS_Z] * M_DRV_MC34X6_GRAVITY) / (M_DRV_MC34X6_GAIN_16BIT/_bPrjCfgRange));
             break;
        case E_M_DRV_MC34X6_RESOLUTION_8BIT:
             faOutput[M_DRV_MC34X6_AXIS_X] = (float)((_waRaw[M_DRV_MC34X6_AXIS_X] * M_DRV_MC34X6_GRAVITY) / (M_DRV_MC34X6_GAIN_8BIT/_bPrjCfgRange));
             faOutput[M_DRV_MC34X6_AXIS_Y] = (float)((_waRaw[M_DRV_MC34X6_AXIS_Y] * M_DRV_MC34X6_GRAVITY) / (M_DRV_MC34X6_GAIN_8BIT/_bPrjCfgRange));
             faOutput[M_DRV_MC34X6_AXIS_Z] = (float)((_waRaw[M_DRV_MC34X6_AXIS_Z] * M_DRV_MC34X6_GRAVITY) / (M_DRV_MC34X6_GAIN_8BIT/_bPrjCfgRange));
             break;
    }

    //If MCU is not support printf for float, need to use ftoi function.
    //M_PRINTF("[[ACC     %4.2f     %4.2f     %4.2f",faOutput[0],faOutput[1],faOutput[2]);  //Acc Raw Data

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_ReadRawData
 *****************************************************************************/
int M_DRV_MC34X6_ReadRawData(short Output[M_DRV_MC34X6_AXES_NUM])
{
    M_PRINTF("[%s] \r\n", __func__);

    _M_DRV_MC34X6_ReadRawData(Output);

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_GetMotion
 *** Do not use this function and HandleINT at same time
 *****************************************************************************/
int    M_DRV_MC34X6_GetMotion(uint8_t *MotionStatus)
{
    M_PRINTF("[%s] \r\n", __func__);

    uint8_t    _bPrjCfgResolution            = M_DRV_MC34X6_CFG_RESOLUTION;
    uint8_t    _bMotion[2]                    = {0};

    if (_bPrjCfgResolution > E_M_DRV_MC34X6_RESOLUTION_16BIT)
    {
        _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_STATUS1 , _bMotion , 2);
        M_PRINTF("0x%02x:%02x 0x%02x:%02x.", E_M_DRV_MC34X6_REG_STATUS1, _bMotion[0], E_M_DRV_MC34X6_REG_INTR_STAT_1, _bMotion[1]);
    }
    else
    {
        _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_STATUS2 , _bMotion , 2);
        M_PRINTF(" 0x%02x:%02x 0x%02x:%02x. ", E_M_DRV_MC34X6_REG_STATUS2, _bMotion[0], E_M_DRV_MC34X6_REG_INTR_STAT_2, _bMotion[1]);
    }

    if (((s_bMotionDetect & 0x1F) == (_bMotion[0] & 0x1F)))
        return (M_DRV_MC34X6_RETCODE_SUCCESS);
    else
    {
        if ((s_bMotionDetect & 0x1F) > (_bMotion[0] & 0x1F))
        {
            s_bMotionDetect = _bMotion[0];
            return (M_DRV_MC34X6_RETCODE_SUCCESS);
        }
        s_bMotionDetect = _bMotion[0];
        if ((_bMotion[0] & 0x1F) >= 0x10)
            MotionStatus[0] = 0x05;
        else if ((_bMotion[0] & 0x1F) >= 0x08)
            MotionStatus[0] = 0x04;
        else if ((_bMotion[0] & 0x1F) >= 0x04)
            MotionStatus[0] = 0x03;
        else if ((_bMotion[0] & 0x1F) >= 0x02)
            MotionStatus[0] = 0x02;
        else if ((_bMotion[0] & 0x1F) >= 0x01)
            MotionStatus[0] = 0x01;
    }

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_HandleINT
 *** Do not use GetMotion and this function at same time
 *****************************************************************************/
int M_DRV_MC34X6_HandleINT(s_m_drv_mc34x6_int_t *ptINT_Event)
{
    M_PRINTF("[%s] \r\n", __func__);

    uint8_t    _bPrjCfgResolution            = M_DRV_MC34X6_CFG_RESOLUTION;
    uint8_t    _bRegStatus2                 = 0;

    if (_bPrjCfgResolution > E_M_DRV_MC34X6_RESOLUTION_16BIT)
        _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_INTR_STAT_1 , &_bRegStatus2 , 1);
    else
        _M_DRV_MC34X6_REG_READ(E_M_DRV_MC34X6_REG_INTR_STAT_2 , &_bRegStatus2 , 1);

    ptINT_Event->bACQ_INT = _M_DRV_MC34X6_REG_INTR_STAT_2_ACQ_INT(_bRegStatus2);
    ptINT_Event->bTILT35_INT = _M_DRV_MC34X6_REG_INTR_STAT_2_TILT35_INT(_bRegStatus2);
    ptINT_Event->bSHAKE_INT = _M_DRV_MC34X6_REG_INTR_STAT_2_SHAKE_INT(_bRegStatus2);
    ptINT_Event->bANYM_INT = _M_DRV_MC34X6_REG_INTR_STAT_2_ANYM_INT(_bRegStatus2);
    ptINT_Event->bFLIP_INT = _M_DRV_MC34X6_REG_INTR_STAT_2_FLIP_INT(_bRegStatus2);
    ptINT_Event->bTILT_INT = _M_DRV_MC34X6_REG_INTR_STAT_2_TILT_INT(_bRegStatus2);
    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_ReadRegMap
 *****************************************************************************/
int M_DRV_MC34X6_ReadRegMap(unsigned char baRegMap[M_DRV_MC34X6_REG_MAP_SIZE])
{
    M_PRINTF("[%s] \r\n", __func__);

    uint8_t    _bIndex = 0;

    for (_bIndex = 0; _bIndex < M_DRV_MC34X6_REG_MAP_SIZE; _bIndex++)
    {
        _M_DRV_MC34X6_REG_READ(_bIndex , (void *)&baRegMap[_bIndex] , 1);
        M_PRINTF("0x%02x = %02x\r\n", _bIndex, baRegMap[_bIndex]);

        _M_DRV_MC34X6_Delay(10240);
    }

    return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/******************************************************************************
 *** M_DRV_MC34X6_GetVersion
 *****************************************************************************/
unsigned long M_DRV_MC34X6_GetVersion(void)
{
    return((((unsigned long)M_DRV_MC34X6_VERSION_AlGOTITHM_MAJOR) << 28) |
           (((unsigned long)M_DRV_MC34X6_VERSION_AlGOTITHM_MINOR) << 24) |
           (((unsigned long)M_DRV_MC34X6_VERSION_AlGOTITHM_BUILD) << 20) |
           (((unsigned long)M_DRV_MC34X6_VERSION_INTERFACE_MAJOR) << 12) |
           (((unsigned long)M_DRV_MC34X6_VERSION_INTERFACE_MINOR) << 8) |
           (((unsigned long)M_DRV_MC34X6_VERSION_INTERFACE_BUILD) << 4));
}