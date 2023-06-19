/* Includes ------------------------------------------------------------------------------------------------*/
#include "LC76F.h"

extern void LC76F_Initialize(struct LC76F * self) {
	LC76F_UART0_Configuration();
	
  self -> lc76f_log_content = (char * ) malloc(LC76F_LOG_CONTENT_SIZE * sizeof(char));
  if (!self -> lc76f_log_content) {
		Error_Blinking_LED_1();
  }
	
  self -> raw_gps_string = (char * ) malloc(LC76F_GPS_DATA_SIZE * sizeof(char));
  if (!self -> raw_gps_string) {
		Error_Blinking_LED_1();
  }
	
  self -> gps_string = (char * ) malloc(LC76F_GPS_DATA_SIZE * sizeof(char));
  if (!self -> gps_string) {
		Error_Blinking_LED_1();
  }
}

void LC76F_UART0_Configuration(void) 
{
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PB = 1;
    CKCUClock.Bit.UART0 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_8, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_7, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_8, AFIO_FUN_USART_UART);

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
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_UART0, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_UART0, ENABLE);
  USART_RxCmd(HT_UART0, ENABLE);
}


void LC76F_UART0_Receive(void)
{
	u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  while (USART_GetFlagStatus(HT_UART0, USART_FLAG_RXDR) == RESET);
  uData = USART_ReceiveData(HT_UART0);

  #if 1 // Loop back Rx data to Tx for test
  Write_Char_Log(uData);
  #endif
}

void LC76F_UART0_Read_Block(char *data)
{
	uint8_t index = 0;
	
	do
	{
  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  while (USART_GetFlagStatus(HT_UART0, USART_FLAG_RXDR) == RESET);
	data[index] = (uint8_t)USART_ReceiveData(HT_UART0);
	}
	while ((data[index] != 0x0A) && (index++ != 99));
}

void Clear_Raw_GPS_String(struct LC76F * self) {
	int i;
	for (i = 0; i < LC76F_GPS_DATA_SIZE; i++) {
		self->raw_gps_string[i] = 0;
	}
}


void Clear_GPS_String(struct LC76F * self) {
	int i;
	for (i = 0; i < LC76F_GPS_DATA_SIZE; i++) {
		self->gps_string[i] = 0;
	}
}

bool Get_GPS_String(struct LC76F * self)
{
	char* check = NULL;
	
	while (!check)
	{
		Clear_Raw_GPS_String(self);
		LC76F_UART0_Read_Block(self->raw_gps_string);
		check = strstr(self->raw_gps_string, "$GNRMC");
	}
	
	/* Sample raw GPS string */
	sprintf(self->raw_gps_string, "$GPRMC,102739.000,A,3150.7825,N,11711.9369,E,0.00,303.62,111214,,,D*6A\r\n");
	strcpy(self->lc76f_log_content, self->raw_gps_string);
	Write_String_Log(self->lc76f_log_content);
	
	if (Check_Valid_GPS_String(self->raw_gps_string)) {
		strcpy(self->gps_string, self->raw_gps_string);
		return true;
	}
	Clear_GPS_String(self);
	return false;
}

bool Check_Valid_GPS_String(char * gps_raw_string) {
	bool valid = false;
	char* check = NULL;
	
	check = strtok(gps_raw_string, ",");
	check = strtok(NULL, ",");
	check = strtok(NULL, ",");
	
	if (strcmp(check, "A") == 0)
	{
		valid = true;
	}
	
	return valid;
}