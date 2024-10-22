#include "MC3416.h"

void MC3416_Initialize(struct MC3416 * self) {
  self -> mc3416_log_content = (char * ) malloc(MC3416_LOG_CONTENT_SIZE * sizeof(char));
  if (!self -> mc3416_log_content) {
		Error_Blinking_LED_1();
  }
	
	MC3416_I2C_Configuration();
	MC3416_Init();
}


void MC3416_I2C_Configuration(void)
{
  { /* Enable peripheral clock                                                                              */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    MC3416_HTCFG_I2C_MASTER_CLK(CKCUClock) = 1;
    CKCUClock.Bit.AFIO              = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Configure GPIO to I2C mode for Master                                                                  */
  AFIO_GPxConfig(MC3416_HTCFG_I2C_MASTER_SCL_GPIO_ID, MC3416_HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_FUN_I2C);
  AFIO_GPxConfig(MC3416_HTCFG_I2C_MASTER_SDA_GPIO_ID, MC3416_HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_FUN_I2C);

  { /* I2C Master configuration                                                                             */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    I2C_InitTypeDef  I2C_InitStructure;

    I2C_InitStructure.I2C_GeneralCall = DISABLE;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;
    I2C_InitStructure.I2C_Acknowledge = DISABLE;
    I2C_InitStructure.I2C_OwnAddress = MC3416_I2C_MASTER_ADDRESS;
    I2C_InitStructure.I2C_Speed = MC3416_CLOCK_SPEED;
    I2C_InitStructure.I2C_SpeedOffset = 0;
    I2C_Init(MC3416_HTCFG_I2C_MASTER_PORT, &I2C_InitStructure);
  }

  /* Enable I2C                                                                                             */
  I2C_Cmd(MC3416_HTCFG_I2C_MASTER_PORT, ENABLE);
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
	MC3416_Write_Mem_Slave(MC3416_ADDR, 0x07, 0x00);
	
	/* Change range and scale: 2G */
	MC3416_Write_Mem_Slave(MC3416_ADDR, 0x20, 0x00);

	/* WAKEUP */
	MC3416_Write_Mem_Slave(MC3416_ADDR, 0x07, 0x01);

	
	while (check != 0x01)
	{
		MC3416_Read_Mem_Slave(MC3416_ADDR, 0x05, &check);
	}
	
}

void MC3416_Read_Accel(struct MC3416 * self, struct Node *current_node)
{
	static uint8_t Rec_Data[6];
	/* Change range and scale */

	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0E, &Rec_Data[0]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0D, &Rec_Data[1]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x10, &Rec_Data[2]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0F, &Rec_Data[3]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x12, &Rec_Data[4]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x11, &Rec_Data[5]);
	
	current_node->accel_x = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]) + MC3416_OFFSET_AX;
	current_node->accel_y = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]) + MC3416_OFFSET_AY;
	current_node->accel_z = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]) + MC3416_OFFSET_AZ;
	
	if (abs(current_node->accel_x) > (TILT_THRESHOLD / 90.0 * (INT16_MAX / 2))) {
		current_node->tilt_alert = true;
	} else {
		current_node->tilt_alert = false;
	}
	
	if (abs(current_node->accel_y) > (WHEELIE_THRESHOLD / 90.0 * (INT16_MAX / 2))) {
		current_node->wheelie_alert = true;
	} else {
		current_node->wheelie_alert = false;
	}
	
	if (TEST_ACCEL) {
		sprintf(self->mc3416_log_content, "Ax %d  |  Ay: %d  |  Az: %d  |  TILT: %d  |  WHEELIE: %d  ", current_node->accel_x, current_node->accel_y, current_node->accel_z, current_node->tilt_alert, current_node->wheelie_alert);
		Write_String_Log(self->mc3416_log_content);
	}
		
}

bool MC3416_Moving(struct MC3416 * self) {
	uint8_t Rec_Data[6];
	/* Change range and scale */
	
	static struct Accel previous_accel;
	static struct Accel current_accel;
	
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0E, &Rec_Data[0]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0D, &Rec_Data[1]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x10, &Rec_Data[2]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0F, &Rec_Data[3]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x12, &Rec_Data[4]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x11, &Rec_Data[5]);

	previous_accel.Ax = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]) + MC3416_OFFSET_AX;
	previous_accel.Ay = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]) + MC3416_OFFSET_AY;
	previous_accel.Az = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]) + MC3416_OFFSET_AZ;
	
	vTaskDelay(200);
	
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0E, &Rec_Data[0]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0D, &Rec_Data[1]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x10, &Rec_Data[2]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x0F, &Rec_Data[3]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x12, &Rec_Data[4]);
	MC3416_Read_Mem_Slave(MC3416_ADDR, 0x11, &Rec_Data[5]);
	
	current_accel.Ax = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]) + MC3416_OFFSET_AX;
	current_accel.Ay = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]) + MC3416_OFFSET_AY;
	current_accel.Az = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]) + MC3416_OFFSET_AZ;
	
	if (abs(current_accel.Ax - previous_accel.Ax) >= MC3416_MOVEMENT_THRESHOLD
			|| abs(current_accel.Ay - previous_accel.Ay) >= MC3416_MOVEMENT_THRESHOLD
			|| abs(current_accel.Az - previous_accel.Az) >= MC3416_MOVEMENT_THRESHOLD) 
	{
		Write_String_Log("Detect movement!");
		return true;
	}
	
	return false;
}