/*********************************************************************************************************//**
 * @file    SRAM_Overwrite/Watchpoint_Heap/main.c
 * @version $Rev:: 6669         $
 * @date    $Date:: 2023-01-17 #$
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
#include <stdlib.h>

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup SRAM_Overwrite_Examples SRAM_Overwrite
  * @{
  */

/** @addtogroup Watchpoint_Heap
  * @{
  */


/* Global variables ----------------------------------------------------------------------------------------*/
u32 TestBuffer[10];

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  s32 *ptrBuffer=malloc(sizeof(s32));
  s32 i;
  *ptrBuffer=0x99;

  RETARGET_Configuration();
  /*Show the initial value from the heap section.*/
  printf("*ptrBuffer address: %p, value:%x\r\n",ptrBuffer,*ptrBuffer);
  /*To overwrite the global array of TestBuffer for covering the heap section.*/
  for(i=0;i<15;i++)
  {
    TestBuffer[i] = i;
    printf("TestBuffer[%d] : %p\r\n", i, &TestBuffer[i]);        
  }
  /*Show the covered value from the heap section.*/
  printf("*ptrBuffer address: %p, value:%x\r\n",ptrBuffer,*ptrBuffer);

  while (1); /* Test Passed                                                                                 */
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
