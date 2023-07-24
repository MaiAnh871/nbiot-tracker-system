/*********************************************************************************************************//**
 * @file    I2C/7_bit_mode_master/main.c
 * @version $Rev:: 4935         $
 * @date    $Date:: 2020-08-26 #$
 * @brief   Main program.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "string.h"
#include "stdlib.h"
#include "E:\STUDY\DATN\HT32\example\I2C\7_bit_mode_master\MDK_ARMv5\m_drv_mc34x6.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup I2C_Examples I2C
  * @{
  */

/** @addtogroup I2C_7_bit_mode_master
  * @{
  */


/* Private types -------------------------------------------------------------------------------------------*/
typedef enum {Fail = 0, Pass = !Fail} TestResult;

/* Private constants ---------------------------------------------------------------------------------------*/
#define I2C_MASTER_ADDRESS     0x0A
#define I2C_SLAVE_ADDRESS      0x4C
#define BufferSize             4
#define ClockSpeed             100000

/*******************************************************************************
 *** CONSTANT / DEFINE
 *******************************************************************************/
#define TF_TEST			0
#define TILT35_TEST		0
#define ANYM_TEST		1
#define SHAKE_TEST		0
#define LPF_TEST		0

/* Private function prototypes -----------------------------------------------------------------------------*/
static void delay_ms(u32 ms);
void I2C_Configuration(void);
void I2C_Master_Transmitter(void);
void I2C_Master_Receiver(void);
void MC3416_Init();
void MC3416_Read_Accel();
void deepsleep(u32 deeptime);

void Initialize_Log(void);
void Write_Char_Log(u16 character);
void Write_String_Log(char * input_string);

TestResult CmpBuffer(u8* Buffer1, u8* Buffer2, u32 BufferLength);

/* Private variables ---------------------------------------------------------------------------------------*/
u8 I2C_Master_Buffer_Tx[BufferSize] = {0x1, 0x2, 0x3, 0x4};
u8 I2C_Master_Buffer_Rx[BufferSize] = {0x00};
vu8 Tx_Index = 0;
vu8 Rx_Index = 0;
char *log_string;
float faOutput[M_DRV_MC34X6_AXES_NUM];

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
static void delay_ms(u32 ms) {
  uint32_t i, j;
  for (i = 0; i < ms; i++) {
    for (j = 0; j < 33132; j++) {
      __NOP();
    }
  }
}

int main(void)
{
  /* Initialize LED1 & LED2 on HT32 board                                                                   */
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);

	Initialize_Log();
	log_string = malloc(500 * sizeof(char));
	
	Write_String_Log("Xin chao cong chua");
	
  I2C_Configuration();                /* I2C configuration                                                  */

  I2C_Master_Transmitter();

  I2C_Master_Receiver();
	
	MC3416_Init();
	
	Write_String_Log("Helloooo");
	
	sprintf(log_string, "%d/%d/%d", 8, 7, 2001);
	Write_String_Log(log_string);
	
  while (1) {
		MC3416_Read_Accel();
		delay_ms(100);
		deepsleep(20);
		MC3416_Read_Accel();
	}
}

void deepsleep(u32 deeptime)
{
		#if (EXAMPLE_RTC_LSE == 1)
	{
		u32 wRtcCounterTmp = 0;
		/* Enable the RTC Compare Match wakeup event                                                              */
		EXTI_WakeupEventIntConfig(ENABLE);
		RTC_WakeupConfig(RTC_WAKEUP_CM, ENABLE);
		/* Wait till RTC count occurs                                                                         */
		wRtcCounterTmp = RTC_GetCounter();
		while (RTC_GetCounter() == wRtcCounterTmp);
	
		/* Compare Match in 3 second                                                                          */
		RTC_SetCompare(RTC_GetCounter()+ deeptime);
	}
	#endif
	PWRCU_DeepSleep1(PWRCU_SLEEP_ENTRY_WFI);
}

void Initialize_Log(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PA = 1;
    CKCUClock.Bit.USART1 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_5, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_5, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {
      0
    };
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_USART1, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_USART1, ENABLE);
  USART_RxCmd(HT_USART1, ENABLE);
}

void Write_Char_Log(u16 character) {
  while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXC) == RESET);
  USART_SendData(HT_USART1, character);
}

void Write_String_Log(char * input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    Write_Char_Log(input_string[i]);
  }
	Write_Char_Log('\n');
}

/*********************************************************************************************************//**
  * @brief  Configure the I2C.
  * @retval None
  ***********************************************************************************************************/
void I2C_Configuration(void)
{
  { /* Enable peripheral clock                                                                              */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    HTCFG_I2C_MASTER_CLK(CKCUClock) = 1;
    CKCUClock.Bit.AFIO              = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Configure GPIO to I2C mode for Master                                                                  */
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_FUN_I2C);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SDA_GPIO_ID, HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_FUN_I2C);

  { /* I2C Master configuration                                                                             */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    I2C_InitTypeDef  I2C_InitStructure;

    I2C_InitStructure.I2C_GeneralCall = DISABLE;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;
    I2C_InitStructure.I2C_Acknowledge = DISABLE;
    I2C_InitStructure.I2C_OwnAddress = I2C_MASTER_ADDRESS;
    I2C_InitStructure.I2C_Speed = ClockSpeed;
    I2C_InitStructure.I2C_SpeedOffset = 0;
    I2C_Init(HTCFG_I2C_MASTER_PORT, &I2C_InitStructure);
  }

  /* Enable I2C                                                                                             */
  I2C_Cmd(HTCFG_I2C_MASTER_PORT, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  I2C master transfers data to another device.
  * @retval None
  ***********************************************************************************************************/
void I2C_Master_Transmitter(void)
{
  /*--------------------------------------------------------------------------------------------------------*/
  /* Master Transmitter                                                                                     */
  /*--------------------------------------------------------------------------------------------------------*/
  /* Send I2C START & I2C slave address for write                                                           */
  I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_SLAVE_ADDRESS, I2C_MASTER_WRITE);

  /* Check on Master Transmitter STA condition and clear it                                                 */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_SEND_START));

  /* Check on Master Transmitter ADRS condition and clear it                                                */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TRANSMITTER_MODE));
  /* Send data                                                                                              */
  while (Tx_Index < BufferSize)
  {
    /* Check on Master Transmitter TXDE condition                                                           */
    while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TX_EMPTY));
    /* Master Send I2C data                                                                                 */
    I2C_SendData(HTCFG_I2C_MASTER_PORT, I2C_Master_Buffer_Tx[Tx_Index ++]);
  }
  /* Send I2C STOP condition                                                                                */
  I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);
  /*wait for BUSBUSY become idle                                                                            */
  while (I2C_ReadRegister(HTCFG_I2C_MASTER_PORT, I2C_REGISTER_SR)&0x80000);
  HT32F_DVB_LEDOn(HT_LED1);
}

/*********************************************************************************************************//**
  * @brief  I2C master receives data from another device.
  * @retval None
  ***********************************************************************************************************/
void I2C_Master_Receiver(void)
{
  /*--------------------------------------------------------------------------------------------------------*/
  /* Master Receiver                                                                                        */
  /*--------------------------------------------------------------------------------------------------------*/
  /* Send I2C START & I2C slave address for read                                                            */
  I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_SLAVE_ADDRESS, I2C_MASTER_READ);

  /* Check on Master Transmitter STA condition and clear it                                                 */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_SEND_START));

  /* Check on Master Transmitter ADRS condition and clear it                                                */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RECEIVER_MODE));

  I2C_AckCmd(HTCFG_I2C_MASTER_PORT, ENABLE);
  /* Receive data                                                                                           */
  while (Rx_Index < BufferSize)
  {

    /* Check on Slave Receiver RXDNE condition                                                              */
    while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RX_NOT_EMPTY));
    /* Store received data on I2C1                                                                          */
    I2C_Master_Buffer_Rx[Rx_Index ++] = I2C_ReceiveData(HTCFG_I2C_MASTER_PORT);
    if (Rx_Index == (BufferSize-1))
    {
      I2C_AckCmd(HTCFG_I2C_MASTER_PORT, DISABLE);
    }
  }
  /* Send I2C STOP condition                                                                                */
  I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);
  /*wait for BUSBUSY become idle                                                                            */
  while (I2C_ReadRegister(HTCFG_I2C_MASTER_PORT, I2C_REGISTER_SR)&0x80000);

  /* Check on validity of received data on I2Cx                                                             */
  if (CmpBuffer(I2C_Master_Buffer_Rx, I2C_Master_Buffer_Tx, BufferSize))
  {
    HT32F_DVB_LEDOff(HT_LED1);
  }
  else
  {
    HT32F_DVB_LEDOff(HT_LED2);
  }
}

void MC3416_Read_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t* str_data)
{
	I2C_TargetAddressConfig(HT_I2C0, Slave_Adr, I2C_MASTER_WRITE);
	/* Check on Master Transmitter STA condition and clear it */
  while (!(HT_I2C0->SR == I2C_MASTER_SEND_START));
  /* Check on Master Transmitter ADRS condition and clear it */
  while (!(HT_I2C0->SR == I2C_MASTER_TRANSMITTER_MODE));
	
	while (!(HT_I2C0->SR == I2C_MASTER_TX_EMPTY));
	I2C_SendData(HT_I2C0, RegAddr);

	while (!(HT_I2C0->SR == I2C_MASTER_TX_EMPTY));
	I2C_TargetAddressConfig(HT_I2C0, Slave_Adr, I2C_MASTER_READ);
	/* Check on Master Transmitter STA condition and clear it                                                 */
  while (!(HT_I2C0->SR == I2C_MASTER_SEND_START));
  /* Check on Master Transmitter ADRS condition and clear it                                                */
  while (!(HT_I2C0->SR == I2C_MASTER_RECEIVER_MODE));

	while (!(HT_I2C0->SR == I2C_MASTER_RX_NOT_EMPTY));
	*str_data = HT_I2C0->DR;
	
	I2C_GenerateSTOP(HT_I2C0);
	
	while (I2C_ReadRegister(HT_I2C0, I2C_REGISTER_SR)&0x80000);
}

void MC3416_Write_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t data)
{
	I2C_TargetAddressConfig(HT_I2C0, Slave_Adr, I2C_MASTER_WRITE);
	/* Check on Master Transmitter STA condition and clear it */
  while (!(HT_I2C0->SR == I2C_MASTER_SEND_START));
  /* Check on Master Transmitter ADRS condition and clear it */
  while (!(HT_I2C0->SR == I2C_MASTER_TRANSMITTER_MODE));

	I2C_SendData(HT_I2C0, RegAddr);
  while (!(HT_I2C0->SR == I2C_MASTER_TX_EMPTY));
	
	I2C_SendData(HT_I2C0, data);
  while (!(HT_I2C0->SR == I2C_MASTER_TX_EMPTY));

	/* Send I2C STOP condition */
  I2C_GenerateSTOP(HT_I2C0);
  /*wait for BUSBUSY become idle */
  while (I2C_ReadRegister(HT_I2C0, I2C_REGISTER_SR)&0x80000);
}

void MC3416_Init(void)
{
	uint8_t check = 0x00;
	/* STANDBY */
	MC3416_Write_Mem_Slave(I2C_SLAVE_ADDRESS, 0x07, 0x00);
	
	/* Change range and scale: 2G */
	MC3416_Write_Mem_Slave(I2C_SLAVE_ADDRESS, 0x20, 0x00);

	/* WAKEUP */
	MC3416_Write_Mem_Slave(I2C_SLAVE_ADDRESS, 0x07, 0x01);

	
	while (check != 0x01)
	{
		MC3416_Read_Mem_Slave(I2C_SLAVE_ADDRESS, 0x05, &check);
	}
	
}

void MC3416_Read_Accel()
{
	static uint8_t Rec_Data[6];
	/* Change range and scale */

	MC3416_Read_Mem_Slave(I2C_SLAVE_ADDRESS, 0x0E, &Rec_Data[0]);
	MC3416_Read_Mem_Slave(I2C_SLAVE_ADDRESS, 0x0D, &Rec_Data[1]);
	MC3416_Read_Mem_Slave(I2C_SLAVE_ADDRESS, 0x10, &Rec_Data[2]);
	MC3416_Read_Mem_Slave(I2C_SLAVE_ADDRESS, 0x0F, &Rec_Data[3]);
	MC3416_Read_Mem_Slave(I2C_SLAVE_ADDRESS, 0x12, &Rec_Data[4]);
	MC3416_Read_Mem_Slave(I2C_SLAVE_ADDRESS, 0x11, &Rec_Data[5]);
	
	int16_t accel_x = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]) + 25;
	int16_t accel_y = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]) + 254;
	int16_t accel_z = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]) + 24;
	
	bool tilt_alert, wheelie_alert;
	
	if (abs(accel_x) > (45 / 90.0 * (INT16_MAX / 2))) {
		bool tilt_alert = true;
	} else {
		bool tilt_alert = false;
	}
	
	if (abs(accel_y) > (30 / 90.0 * (INT16_MAX / 2))) {
		bool wheelie_alert = true;
	} else {
		bool wheelie_alert = false;
	}
	
	if (1) {
		sprintf(log_string, "Ax %d  |  Ay: %d  |  Az: %d  |  TILT: %d  |  WHEELIE: %d  ", accel_x, accel_y, accel_z, tilt_alert, wheelie_alert);
		Write_String_Log(log_string);
	}
		
}

/*********************************************************************
 *** _M_DRV_ANYM_Init
 *** Initial process for ANYM mode
 *********************************************************************/
int _M_DRV_ANYM_Init(void)
{
	//STANDBY
	M_DRV_MC34X6_SetMode(E_M_DRV_MC34X6_MODE_STANDBY);

	//set ANYM Threshold
	M_DRV_MC34X6_SetANYMThreshold();

	//set ANYM Debounce
	M_DRV_MC34X6_SetANYMDebounce();

	//set ANYM Int
	M_DRV_MC34X6_ConfigINT(E_M_DRV_MC34X6_INT_ANYM_EN | E_M_DRV_MC34X6_INT_AUTOCLR_EN);	//ANYM+auto

	//set Motion Control
	M_DRV_MC34X6_ConfigMotionCtl(E_M_DRV_MC34X6_MCTL_ANYM_EN);

	return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/*********************************************************************
 *** M_DRV_Mode_Event
 *********************************************************************/
int M_DRV_Mode_Event(void)
{
	unsigned char _baBuf[2] = {0};

	_M_DRV_MC34X6_Delay(10240);
	_M_DRV_MC34X6_Delay(10240);
	_M_DRV_MC34X6_Delay(10240);
	_M_DRV_MC34X6_Delay(10240);
	_M_DRV_MC34X6_Delay(10240);
	M_DRV_MC34X6_GetMotion(&_baBuf[0]);

	switch(_baBuf[0])
	{
		case 5:
			Write_String_Log("-------------------- TILT35 --------------------");
			break;
		case 4:
			Write_String_Log("-------------------- SHAKE --------------------");
			break;
		case 3:
			Write_String_Log("-------------------- ANYM --------------------");
			break;
		case 2:
			Write_String_Log("-------------------- FLIP --------------------");
			break;
		case 1:
			Write_String_Log("-------------------- TILT --------------------");
			break;
	}
	return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/*********************************************************************
 *** M_DRV_MC34X6_Event_Init
 *** Sensor Initialize
 *** Select function between TF or Shake
 *********************************************************************/
int M_DRV_MC34X6_Event_Init(void)
{

#if TF_TEST
	_M_DRV_Tilt_Flip_Init();	//Tilt_Flip_Test
#elif TILT35_TEST
	_M_DRV_Tilt35_Init();		//Tilt35_Test
#elif ANYM_TEST
	_M_DRV_ANYM_Init();			//ANYM_Test
#elif SHAKE_TEST
	_M_DRV_Shake_Init(); 		//Shake_Test
#elif LPF_TEST
	_M_DRV_LPF_Init(); 			//LPF_Test
#endif

	return (M_DRV_MC34X6_RETCODE_SUCCESS);
}

/*********************************************************************************************************//**
  * @brief  Compare two buffers.
  * @param  Buffer1, Buffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval None
  ***********************************************************************************************************/
TestResult CmpBuffer(u8* Buffer1, u8* Buffer2, u32 BufferLength)
{
  while (BufferLength--)
  {
    if (*Buffer1 != *Buffer2)
    {
      return Fail;
    }

    Buffer1++;
    Buffer2++;
  }

  return Pass;
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
