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
	
  self -> temp = (char * ) malloc(LC76F_GPS_DATA_SIZE * sizeof(char));
  if (!self -> temp) {
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

bool Check_Valid_GPS_String(struct LC76F * self) {
	bool valid = false;
	char* check = NULL;

	strcpy(self->temp, self->raw_gps_string);
	
	check = strtok(self->temp, ",");
	check = strtok(NULL, ",");
	check = strtok(NULL, ",");
	
	if (strcmp(check, "A") == 0)
	{
		valid = true;
	}

	return valid;
}

void Clear_Temp(struct LC76F * self) {
	int i;
	for (i = 0; i < LC76F_TEMP_STRING_LENGTH; i++) {
		self->temp[i] = 0;
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
	if (TEST_GPS_STRING) {
		Write_String_Log("Using fake GPS string!");
		sprintf(self->raw_gps_string, "$GNRMC,045910.817,A,2101.799402,N,10546.931885,E,0.00,0.00,040723,,,A,V*04\r\n");
	}
	
	if (Check_Valid_GPS_String(self)) {
		strcpy(self->gps_string, self->raw_gps_string);
		return true;
	}
	Clear_GPS_String(self);
	return false;
}

bool Parse_GPS_String(struct LC76F * self, struct Node *current_node) {
	if (!current_node) {
		Write_String_Log("Parse_GPS_String: <current_node> is NULL!\n");
		return false;
	}
	
	uint8_t num_tokens;
	char **token_array;
	
	token_array = Tokenize_String(self->gps_string, ",", &num_tokens);
	
	/* Time */
	Clear_Temp(self);
	slice(token_array[1], self->temp, 0, 2);
	current_node->timestamp.hour = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[1], self->temp, 2, 4);
	current_node->timestamp.minute = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[1], self->temp, 4, 6);
	current_node->timestamp.second = atoi(self->temp);
	
	/* Latitude */
	Clear_Temp(self);
	slice(token_array[3], self->temp, 0, 2);
	current_node->latitude.degree = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[3], self->temp, 2, 4);
	current_node->latitude.minute = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[3], self->temp, 5, 9);
	current_node->latitude.second = atoi(self->temp);
	
	/* North-South */
	slice(token_array[4], self->temp, 0, 1);
	if (self->temp[0] == 'N'){
		current_node->latitude.latitude_direction = NORTH;
	} else if (self->temp[0] == 'S') {
		current_node->latitude.latitude_direction = SOUTH;
	} else {
		current_node->valid = false;
		return current_node->valid;
	}
	
	/* Longitude */
	Clear_Temp(self);
	slice(token_array[5], self->temp, 0, 3);
	current_node->longitude.degree = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[5], self->temp, 3, 5);
	current_node->longitude.minute = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[5], self->temp, 6, 10);
	current_node->longitude.second = atoi(self->temp);
	
	/* East-West*/
	slice(token_array[6], self->temp, 0, 1);
	if (self->temp[0] == 'E'){
		current_node->longitude.longitude_direction = EAST;
	} else if (self->temp[0] == 'W') {
		current_node->longitude.longitude_direction = WEST;
	} else {
		current_node->valid = false;
		return current_node->valid;
	}
	
	/* Date */
	Clear_Temp(self);
	slice(token_array[9], self->temp, 0, 2);
	current_node->timestamp.day = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[9], self->temp, 2, 4);
	current_node->timestamp.month = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[9], self->temp, 4, 6);
	current_node->timestamp.year = atoi(self->temp);
	
//	sprintf(self->lc76f_log_content, "DAY: %u\n", self->node.timestamp.day);
//	Write_String_Log(self->lc76f_log_content);
//	sprintf(self->lc76f_log_content, "MONTH: %u\n", self->node.timestamp.month);
//	Write_String_Log(self->lc76f_log_content);
//	sprintf(self->lc76f_log_content, "YEAR: %u\n", self->node.timestamp.year);
//	Write_String_Log(self->lc76f_log_content);
	
//	int i;
//	for (i = 0; i < num_tokens; i++) {
//		sprintf(self->lc76f_log_content, "%s\n", token_array[i]);
//		Write_String_Log(self->lc76f_log_content);
//	}
	
	free(token_array);
	
	current_node->valid = true;
	
	return current_node->valid;
}