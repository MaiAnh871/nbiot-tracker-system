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
	
	self->update_previous_coordinates = true;
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
	
	vTaskDelay(1);
		
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

void Parse_GPS_String(struct LC76F * self, struct Node *current_node) {	
	uint8_t num_tokens;
	char **token_array;
	token_array = Tokenize_String(self->gps_string, ",", &num_tokens);
	struct Coordinates temp_coordinates;
	
//	int i;
//	for (i = 0; i < num_tokens; i++) {
//		sprintf(self->lc76f_log_content, "%s\n", token_array[i]);
//		Write_String_Log(self->lc76f_log_content);
//	}		
	
	/* Time */
	Clear_Temp(self);
	slice(token_array[1], self->temp, 0, 2);
	temp_coordinates.timestamp.hour = ((atoi(self->temp) + TIME_ZONE) % 24);
	Clear_Temp(self);
	slice(token_array[1], self->temp, 2, 4);
	temp_coordinates.timestamp.minute = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[1], self->temp, 4, 6);
	temp_coordinates.timestamp.second = atoi(self->temp);
	
	/* Latitude */
	Clear_Temp(self);
	slice(token_array[3], self->temp, 0, 2);
	temp_coordinates.latitude.degree = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[3], self->temp, 2, 4);
	temp_coordinates.latitude.minute = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[3], self->temp, 5, 9);
	temp_coordinates.latitude.second = atoi(self->temp);
	
	/* North-South */
	slice(token_array[4], self->temp, 0, 1);
	if (self->temp[0] == 'N'){
		temp_coordinates.latitude.latitude_direction = NORTH;
	} else if (self->temp[0] == 'S') {
		temp_coordinates.latitude.latitude_direction = SOUTH;
	} else {
		return;
	}
	
	/* North-South */
	slice(token_array[4], self->temp, 0, 1);
	if (self->temp[0] == 'N'){
		temp_coordinates.latitude.latitude_direction = NORTH;
	} else if (self->temp[0] == 'S') {
		temp_coordinates.latitude.latitude_direction = SOUTH;
	} else {
		return;
	}
	
	/* Longitude */
	Clear_Temp(self);
	slice(token_array[5], self->temp, 0, 3);
	temp_coordinates.longitude.degree = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[5], self->temp, 3, 5);
	temp_coordinates.longitude.minute = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[5], self->temp, 6, 10);
	temp_coordinates.longitude.second = atoi(self->temp);	
	
	/* East-West*/
	slice(token_array[6], self->temp, 0, 1);
	if (self->temp[0] == 'E'){
		temp_coordinates.longitude.longitude_direction = EAST;
	} else if (self->temp[0] == 'W') {
		temp_coordinates.longitude.longitude_direction = WEST;
	} else {
		return;
	}	
	
	/* Date */
	Clear_Temp(self);
	slice(token_array[9], self->temp, 0, 2);
	temp_coordinates.timestamp.day = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[9], self->temp, 2, 4);
	temp_coordinates.timestamp.month = atoi(self->temp);
	Clear_Temp(self);
	slice(token_array[9], self->temp, 4, 6);
	temp_coordinates.timestamp.year = atoi(self->temp);	
	
	free(token_array);
	
	if (self->update_previous_coordinates) {	
		self->previous_coordinates = temp_coordinates;
		self->update_previous_coordinates = false;
		return;
	}
	
//	sprintf(self->lc76f_log_content, ",\"MEASURING timestamp\":\"%u:%u:%uT%u-%u-%u\"", self->current_coordinates.timestamp.hour, self->current_coordinates.timestamp.minute, self->current_coordinates.timestamp.second, self->current_coordinates.timestamp.day, self->current_coordinates.timestamp.month, self->current_coordinates.timestamp.year);
//	Write_String_Log(self->lc76f_log_content);
	
	self->current_coordinates = temp_coordinates;
	
	float speed = Calculate_Speed(self);
	
	self->previous_coordinates = self->current_coordinates;
	
	if (!current_node) {
		Write_String_Log("Parse_GPS_String: <current_node> is NULL!\n");
		return;
	}
	
	current_node->coordinates = self->current_coordinates;
	current_node->speed = speed;
	current_node->valid = true;
}

float DMS_To_Decimal(uint8_t degree, uint8_t minute, uint16_t second, int8_t sign) {
	float output = sign * ((float) degree + (float) minute / (60.0) + (float) second / 600000.0);
	return output;
}

float Degree_To_Rad(float degree) {
    return degree * PI / 180.0;
}

float Calculate_Distance(struct LC76F * self) {
	float lat_1 = Degree_To_Rad(DMS_To_Decimal(self->previous_coordinates.latitude.degree, self->previous_coordinates.latitude.minute, self->previous_coordinates.latitude.second, self->previous_coordinates.latitude.latitude_direction));
	float lon_1 = Degree_To_Rad(DMS_To_Decimal(self->previous_coordinates.longitude.degree, self->previous_coordinates.longitude.minute, self->previous_coordinates.longitude.second, self->previous_coordinates.longitude.longitude_direction));
	float lat_2 = Degree_To_Rad(DMS_To_Decimal(self->current_coordinates.latitude.degree, self->current_coordinates.latitude.minute, self->current_coordinates.latitude.second, self->current_coordinates.latitude.latitude_direction));
	float lon_2 = Degree_To_Rad(DMS_To_Decimal(self->current_coordinates.longitude.degree, self->current_coordinates.longitude.minute, self->current_coordinates.longitude.second, self->current_coordinates.longitude.longitude_direction));
								
	// Calculate differences between the coordinates
	float dlat = lat_2 - lat_1;
	float dlon = lon_2 - lon_1;
	
	// Apply Haversine formula
	float a, b, c;
	a = sin(dlat / 2) * sin(dlat / 2) + cos(lat_1) * cos(lat_2) * sin(dlon / 2) * sin(dlon / 2);
	c = 2 * atan2(sqrt(a), sqrt(1 - a));
	return (float) (EARTH_RADIUS * c);
}

uint32_t Calculate_Time(struct LC76F * self) {
	struct Timestamp timestamp_1 = self->previous_coordinates.timestamp;
	struct Timestamp timestamp_2 = self->current_coordinates.timestamp;
	
	struct tm start_time;
	start_time.tm_hour = timestamp_1.hour;    // Hour (0-23)
	start_time.tm_min = timestamp_1.minute;     // Minute (0-59)
	start_time.tm_sec = timestamp_1.second;     // Second (0-59)
	start_time.tm_mday = timestamp_1.day;    // Day of the month (1-31)
	start_time.tm_mon = timestamp_1.month;      // Month (0-11, January = 0)
	start_time.tm_year = timestamp_1.year + 2000 - 1900;   // Year since 1900

	// Define the second timestamp
	struct tm end_time;
	end_time.tm_hour = timestamp_2.hour;      // Hour (0-23)
	end_time.tm_min = timestamp_2.minute;       // Minute (0-59)
	end_time.tm_sec = timestamp_2.second;       // Second (0-59)
	end_time.tm_mday = timestamp_2.day;      // Day of the month (1-31)
	end_time.tm_mon = timestamp_2.month;        // Month (0-11, January = 0)
	end_time.tm_year = timestamp_2.year + 2000 - 1900;     // Year since 1900
	
	sprintf(self->lc76f_log_content, "%u:%u:%u", timestamp_2.hour, timestamp_2.minute, timestamp_2.second);
	Write_String_Log(self->lc76f_log_content);
	
	// Convert the timestamps to time_t values
	time_t start_t = mktime(&start_time);
	time_t end_t = mktime(&end_time);

	// Calculate the time interval in seconds
	time_t interval = end_t - start_t;

	return interval;
}

float Calculate_Speed(struct LC76F * self) {
	float distance = Calculate_Distance(self);
	float time_interval = (float) Calculate_Time(self);
	float speed = distance / time_interval;
	sprintf(self->lc76f_log_content, "DISTANCE: %f, TIME_INTERVAL: %f, SPEED: %f", distance, time_interval, speed);
	Write_String_Log(self->lc76f_log_content);
	return speed;
}