/* Includes ------------------------------------------------------------------------------------------------*/
#include "MC3416.h"

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

void Read_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t* str_data)
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

void Write_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t data)
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
	/* Wake up: all setting default */
	Write_Mem_Slave(MC3416_ADDR, 0x07, 0x01);
	
	while (check != 0x01)
	{
		Read_Mem_Slave(MC3416_ADDR, 0x05, &check);
	}
}

void MC3416_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az)
{
	static uint8_t Rec_Data[6];
	//Read_Mem_Slave(MC3416_ADDR, 0x75, Rec_Data, 6);

	Read_Mem_Slave(MC3416_ADDR, 0x0E, &Rec_Data[0]);
	Read_Mem_Slave(MC3416_ADDR, 0x0D, &Rec_Data[1]);
	Read_Mem_Slave(MC3416_ADDR, 0x10, &Rec_Data[2]);
	Read_Mem_Slave(MC3416_ADDR, 0x0F, &Rec_Data[3]);
	Read_Mem_Slave(MC3416_ADDR, 0x12, &Rec_Data[4]);
	Read_Mem_Slave(MC3416_ADDR, 0x11, &Rec_Data[5]);
	
	*Ax = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	*Ay = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	*Az = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);
}