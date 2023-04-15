/*********************************************************************************************************//**
 * @file    IP/Example/main.c
 * @version $Rev:: 4869         $
 * @date    $Date:: 2020-08-05 #$
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
// <<< Use Configuration Wizard in Context Menu >>>

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"

#define USART_FLAG_TXE ((u16)0x0080)
#define USART_FLAG_RXNE ((uint16_t)0x0020)

#define HTCFG_UART_PORT 									HT_USART0
#define HTCFG_TX_PDMA_CH 									PDMA_USART0_TX
#define HTCFG_RX_PDMA_CH 									PDMA_USART0_RX
#define HTCFG_PDMA_IRQ 										PDMACH2_5_IRQn 
#define HTCFG_PDMA_IRQHandler             (PDMA_CH2_5_IRQHandler)
#define HTCFG_UART_RX_GPIO_CLK 						PA
#define HTCFG_UART_IPN										USART0
#define HTCFG_UART_RX_GPIO_PORT 					HT_GPIOA
#define HTCFG_UART_RX_GPIO_PIN  					GPIO_PIN_3
#define HTCFG_UART_TX_GPIO_ID							GPIO_PA
#define HTCFG_UART_TX_AFIO_PIN						AFIO_PIN_2
#define HTCFG_UART_RX_GPIO_ID							GPIO_PA
#define HTCFG_UART_RX_AFIO_PIN						AFIO_PIN_3
#define HTCFG_PDMA_CURRENT_TRANSFER_SIZE  (HT_PDMA->PDMACH2.CTSR >> 16)


/** @addtogroup Project_Template Project Template
  * @{
  */

/** @addtogroup IP_Examples IP
  * @{
  */

/** @addtogroup Example
  * @{
  */


/* Settings ------------------------------------------------------------------------------------------------*/
#define RX_BUFFER_SIZE                            (64)
#define RX_FULL_CHECK                             (0)

/* Private types -------------------------------------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/
void PDMA_Configuration(void);
void UxART_Configuration(void);
u32 UxART_PDMA_Tx(uc8 *TxBuffer, u32 length);
u32 UxART_PDMA_RxReadByte(u8* pBuf);
u32 UxART_PDMA_RxReadBlock(u8* pBuf, u32 uLen);
u32 UxART_PDMA_RxGetLength(void);
u32 UxART_PDMA_RxIsFull(void);
void UxART_RxMainRoutine(void);

void UxART1_Configuration(void);
void UxART1_TxSend(u16 Data);
void UxART1_TxTest(void);
void UxART1_RxTest_Block(void);
void UxART1_RxTest_NonBlock(void);


void LED_Init(void);
void LED_Toggle(void);

static void __Delay(u32 count);

/* Private macro -------------------------------------------------------------------------------------------*/
#define GetRxBufferReadIndex()                    gRxBufferReadIndex
#define GetRxBufferWriteIndex()                   (RX_BUFFER_SIZE - HTCFG_PDMA_CURRENT_TRANSFER_SIZE)
#define IsRxBufferEmpty()                         (GetRxBufferWriteIndex() == GetRxBufferReadIndex())
#define IsRxBufferFull()                          ((GetRxBufferWriteIndex() + 1) == RX_BUFFER_SIZE ? (0 == GetRxBufferReadIndex()) : ((GetRxBufferWriteIndex() + 1) == GetRxBufferReadIndex()))

/* Global variables ----------------------------------------------------------------------------------------*/
PDMACH_InitTypeDef gPDMACH_TxStructure;
PDMACH_InitTypeDef gPDMACH_RxStructure;

vu32 gIsUxART_PDMA_TxBusy = FALSE;

u8 gRxBuffer[RX_BUFFER_SIZE];

/* Private variables ---------------------------------------------------------------------------------------*/
u32 gRxBufferReadIndex = 0;
uc8 gHelloString[] = "Hello, this is USART Tx/Rx PDMA example. Please enter characters.....\r\n";

u8 gTempBuffer[64];

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  LED_Init();
	LED_Toggle();
  PDMA_Configuration();
  UxART_Configuration();
	UxART1_Configuration();
	
	/* Send "Hello, world!" over UART1 */
  char *message = "Hello, world!\r\n";
  while (*message) {
    USART_SendData(HT_USART1, (uint8_t)*message++);
    while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXE) == RESET);
  }

  /* Send hello information by PDMA mode                                                                    */
  UxART_PDMA_Tx(gHelloString, sizeof(gHelloString) - 1);

  while (1)
  {
    UxART_RxMainRoutine();
  }
	LED_Toggle();
}

/*********************************************************************************************************//**
  * @brief  Configure the PDMA.
  * @retval None
  ***********************************************************************************************************/
void PDMA_Configuration(void)
{
  /* Enable peripheral clock of PDMA                                                                        */
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PDMA   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* Tx PDMA channel configuration                                                                          */
  gPDMACH_TxStructure.PDMACH_SrcAddr = (u32)NULL;
  gPDMACH_TxStructure.PDMACH_DstAddr = (u32)&HTCFG_UART_PORT->DR;
  gPDMACH_TxStructure.PDMACH_BlkCnt = 0;
  gPDMACH_TxStructure.PDMACH_BlkLen = 1;
  gPDMACH_TxStructure.PDMACH_DataSize = WIDTH_8BIT;
  gPDMACH_TxStructure.PDMACH_Priority = M_PRIO;
  gPDMACH_TxStructure.PDMACH_AdrMod = SRC_ADR_LIN_INC | DST_ADR_FIX;
  #if 0 // Config and Enable DMA when Start Tx
  PDMA_Config(HTCFG_TX_PDMA_CH, &gPDMACH_TxStructure);
  PDMA_EnaCmd(HTCFG_TX_PDMA_CH, ENABLE);
  #endif
  PDMA_IntConfig(HTCFG_TX_PDMA_CH, PDMA_INT_GE | PDMA_INT_TC, ENABLE);

  /* Rx PDMA channel configuration                                                                          */
  gPDMACH_RxStructure.PDMACH_SrcAddr = (u32)&HTCFG_UART_PORT->DR;
  gPDMACH_RxStructure.PDMACH_DstAddr = (u32)gRxBuffer;
  gPDMACH_RxStructure.PDMACH_BlkCnt = RX_BUFFER_SIZE;
  gPDMACH_RxStructure.PDMACH_BlkLen = 1;
  gPDMACH_RxStructure.PDMACH_DataSize = WIDTH_8BIT;
  gPDMACH_RxStructure.PDMACH_Priority = H_PRIO;
  gPDMACH_RxStructure.PDMACH_AdrMod = SRC_ADR_FIX | DST_ADR_LIN_INC | AUTO_RELOAD;
  PDMA_Config(HTCFG_RX_PDMA_CH, &gPDMACH_RxStructure);
  PDMA_EnaCmd(HTCFG_RX_PDMA_CH, ENABLE);
  #if 0 // Auto Reload by PDMA
  PDMA_IntConfig(HTCFG_RX_PDMA_CH, PDMA_INT_GE | PDMA_INT_TC, ENABLE);
  #endif

  #if (RX_FULL_CHECK == 1)
  PDMA_IntConfig(HTCFG_RX_PDMA_CH, PDMA_INT_GE | PDMA_INT_BE, ENABLE);
  #endif

  NVIC_EnableIRQ(HTCFG_PDMA_IRQ);
}

/*************************************************************************************************************
  * @brief  Configure the UxART
  * @retval None
  ***********************************************************************************************************/
void UxART_Configuration(void)
{
  #if 0 // Use following function to configure the IP clock speed.
  // The UxART IP clock speed must be faster 16x then the baudrate.
  CKCU_SetPeripPrescaler(CKCU_PCLK_UxARTn, CKCU_APBCLKPRE_DIV2);
  #endif

  { /* Enable peripheral clock of AFIO, UxART                                                               */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.AFIO                   = 1;
    CKCUClock.Bit.HTCFG_UART_RX_GPIO_CLK = 1;
    CKCUClock.Bit.HTCFG_UART_IPN         = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HTCFG_UART_RX_GPIO_PORT, HTCFG_UART_RX_GPIO_PIN, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(HTCFG_UART_TX_GPIO_ID, HTCFG_UART_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(HTCFG_UART_RX_GPIO_ID, HTCFG_UART_RX_AFIO_PIN, AFIO_FUN_USART_UART);

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
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HTCFG_UART_PORT, &USART_InitStructure);
  }

  #if 0 // Enable UxART TX DMA when Start Tx
  /* Enable UxART TX trigger DMA                                                                            */
  USART_PDMACmd(HTCFG_UART_PORT, USART_PDMAREQ_TX, ENABLE);
  #endif

  /* Enable UxART RX trigger DMA                                                                            */
  USART_PDMACmd(HTCFG_UART_PORT, USART_PDMAREQ_RX, ENABLE);

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HTCFG_UART_PORT, ENABLE);
  USART_RxCmd(HTCFG_UART_PORT, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  UxART send a buffer by PDMA.
  * @retval None
  ***********************************************************************************************************/
u32 UxART_PDMA_Tx(uc8 *TxBuffer, u32 length)
{
	u32 i;
	
	/* UART0 - MCU TO MODULE
  /* Wait until previou Tx finished                                                                         */
  while (gIsUxART_PDMA_TxBusy == TRUE);

  /* UxART Tx PDMA channel configuration                                                                    */
  gPDMACH_TxStructure.PDMACH_SrcAddr = (u32)TxBuffer;
  gPDMACH_TxStructure.PDMACH_BlkCnt = length;
  PDMA_Config(HTCFG_TX_PDMA_CH, &gPDMACH_TxStructure);
  PDMA_EnaCmd(HTCFG_TX_PDMA_CH, ENABLE);

  gIsUxART_PDMA_TxBusy = TRUE;
  USART_PDMACmd(HTCFG_UART_PORT, USART_PDMAREQ_TX, ENABLE);

	/* UART1 - MCU TO PC
	/* Send a buffer from UxART1 to terminal                                                                   */
  for (i = 0; i < length; i++)
  {
    UxART1_TxSend(TxBuffer[i]);
  }
  return length;
}

/*********************************************************************************************************//**
  * @brief  Read byte from Rx buffer.
  * @retval 0: No Data, 1 Read Success
  ***********************************************************************************************************/
u32 UxART_PDMA_RxReadByte(u8 *pBuf)
{
  if (IsRxBufferEmpty())
  {
    return 0;
  }
  else
  {
    *pBuf = gRxBuffer[gRxBufferReadIndex++];
    if (gRxBufferReadIndex == RX_BUFFER_SIZE)
    {
      gRxBufferReadIndex = 0;
    }
    return 1;
  }
}

/*********************************************************************************************************//**
  * @brief  Read block from Rx buffer.
  * @retval Data length
  ***********************************************************************************************************/
u32 UxART_PDMA_RxReadBlock(u8 *pBuf, u32 uLen)
{
  u32 i = 0;
  for (i = 0; i < uLen; i++)
  {
    if (IsRxBufferEmpty())
    {
      break;
    }
    *pBuf++ = gRxBuffer[gRxBufferReadIndex++];
    if (gRxBufferReadIndex == RX_BUFFER_SIZE)
    {
      gRxBufferReadIndex = 0;
    }
  }
  return i;
}

/*********************************************************************************************************//**
  * @brief  Get Rx data length.
  * @retval Data length
  ***********************************************************************************************************/
u32 UxART_PDMA_RxGetLength(void)
{
  u32 uWrite = GetRxBufferWriteIndex();
  u32 uRead = GetRxBufferReadIndex();

  if (uWrite >= uRead)
  {
    return (uWrite - uRead);
  }
  else
  {
    return (RX_BUFFER_SIZE - uRead + uWrite);
  }
}

/*********************************************************************************************************//**
  * @brief  Check Rx is full or not.
  * @retval 0: Rx is not full, 1: Rx is full
  ***********************************************************************************************************/
u32 UxART_PDMA_RxIsFull(void)
{
  return (IsRxBufferFull());
}

/*********************************************************************************************************//**
  * @brief  UxART Rx Main Routine.
  * @retval None
  ***********************************************************************************************************/
void UxART_RxMainRoutine(void)
{
  #if 1
  if (UxART_PDMA_RxReadByte(gTempBuffer)) // Process data after 1 byte received
  {
    // Do data process here

    #if 1 // Loopback Rx data to Tx
    UxART_PDMA_Tx((uc8 *)&gTempBuffer, 1);
    #endif
  }
  #else
  if (UxART_PDMA_RxGetLength() >= 5)      // Process data after 5 byte received
  {
    u32 uLen;
    uLen = UxART_PDMA_RxReadBlock(gTempBuffer, UxART_PDMA_RxGetLength());
    // Do data process here

    #if 1 // Loopback Rx data to Tx
    UxART_PDMA_Tx((uc8 *)gTempBuffer, uLen);
    #endif
  }
  #endif
}

/*********************************************************************************************************//**
  * @brief  Configure the UART1.
  * @retval None
  ***********************************************************************************************************/
void UxART1_Configuration(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

	{/* Enable peripheral clock of AFIO, UxART                                                                 */
	CKCUClock.Bit.AFIO = 1;
	CKCUClock.Bit.PA = 1;
	CKCUClock.Bit.USART1 = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	}
	
	/* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_4, GPIO_PR_UP);
	
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
    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_USART1, &USART_InitStructure);
  }
	
	/* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_USART1, ENABLE);
  USART_RxCmd(HT_USART1, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  UxART Tx Send Byte.
  * @param  Data: the data to be transmitted.
  * @retval None
  ***********************************************************************************************************/
void UxART1_TxSend(u16 Data)
{
  while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXC) == RESET);
  USART_SendData(HT_USART1, Data);
}

/*********************************************************************************************************//**
  * @brief  UxART Tx Test.
  * @retval None
  ***********************************************************************************************************/
void UxART1_TxTest(void)
{
  u32 i;
  u8 *uPtr = (u8 *)gHelloString;
  u32 uLen = sizeof(gHelloString) - 1;

  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < uLen; i++)
  {
    UxART1_TxSend(uPtr[i]);
  }
}

/*********************************************************************************************************//**
  * @brief  UxART Rx Test - Blocking mode.
  * @retval None
  ***********************************************************************************************************/
void UxART1_RxTest_Block(void)
{
  u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  while (USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDR) == RESET);
  uData = USART_ReceiveData(HT_USART1);

  #if 1 // Loop back Rx data to Tx for test
  UxART1_TxSend(uData);
  #endif
}

/*********************************************************************************************************//**
  * @brief  UxART Rx Test - Non-Blocking mode
  * @retval None
  ***********************************************************************************************************/
void UxART1_RxTest_NonBlock(void)
{
  u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  if (USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDR) == SET)
  {
    uData = USART_ReceiveData(HT_USART1);

    #if 1 // Loop back Rx data to Tx for test
    UxART1_TxSend(uData);
    #endif
  }
}

void LED_Init()
{
	HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
  HT32F_DVB_LEDInit(HT_LED3);
  HT32F_DVB_LEDOn(HT_LED1);
  HT32F_DVB_LEDOff(HT_LED2);
  HT32F_DVB_LEDOn(HT_LED3);
}

void LED_Toggle()
{
	s32 input;
	for (input = 0; input < 10; input++)
  {
    __Delay(2000000);
    HT32F_DVB_LEDToggle(HT_LED1);
    HT32F_DVB_LEDToggle(HT_LED2);
    HT32F_DVB_LEDToggle(HT_LED3);
  }
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

/* Private functions ---------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  delay function
  * @param  count: delay count for loop
  * @retval None
  ***********************************************************************************************************/
static void __Delay(u32 count)
{
  while (count--)
  {
    __NOP(); // Prevent delay loop be optimized
  }
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
