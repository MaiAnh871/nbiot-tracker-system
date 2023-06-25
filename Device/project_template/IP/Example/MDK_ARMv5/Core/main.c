/* Includes */ #include "ht32.h"

#include "ht32_board.h"

#include "stdlib.h"

#include "string.h"

#include <math.h>

#include "Board871.h"

/* Private function prototypes -----------------------------------------------------------------------------*/
//void setup(struct BC660K * self);
//void addCA(struct BC660K * self);
//void loop(struct BC660K * self);

/* AT Command functions */

/* ==================== */

/* UART ports */
void UART0_GNSS_Configuration(void);
void UART0_Receive(void);
void UART0_Read_Block(uint8_t * data);

/* Private macro -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/
struct Board871 board871;

uint8_t data[100];
uint8_t * check = NULL;
uint8_t GPS_raw[100];
float latitude;
float longitude;
float current_lat;
float current_lon;
int16_t Ax = 0;
int16_t Ay = 0;
int16_t Az = 0;

/* Global functions ----------------------------------------------------------------------------------------*/
static void delay_ms(u32 ms);
void LED_Init(void);
void Toggle_LED_1(void);
void Toggle_LED_2(void);


void Error_Blinking_LED_1(void);
void Connecting_Blinking_LED_2(void);
void Initialize_Log(void);
void Write_Char_Log(u16 character);
void Write_String_Log(char * input_string);

void task_1(void * argument);
void task_2(void * argument);
void task_3(void * argument);
void task_4(void * argument);

//void clear(uint8_t * input_string);
//bool getRawGPS(void);
//bool checkValidGPS(uint8_t * raw_GPS);
//void printBool(bool b);
//void extractMainData(void);
//float calculateDistance(void);
//void packMsg(void);

static void delay_ms(u32 ms) {
  uint32_t i, j;
  for (i = 0; i < ms; i++) {
    for (j = 0; j < 33132; j++) {
      __NOP();
    }
  }
}

void LED_Init() {
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
  HT32F_DVB_LEDInit(HT_LED3);
  HT32F_DVB_LEDOff(HT_LED1);
  HT32F_DVB_LEDOff(HT_LED2);
  HT32F_DVB_LEDOff(HT_LED3);
	
	Toggle_LED_1();
	Toggle_LED_2();
}

void Toggle_LED_1() {
  HT32F_DVB_LEDToggle(HT_LED1);
}

void Toggle_LED_2() {
  HT32F_DVB_LEDToggle(HT_LED2);
}

void Error_Blinking_LED_1(void) {
	while (1) {
		Toggle_LED_1();
		delay_ms(200);
	}
}

void Connecting_Blinking_LED_2(void) {
	
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
    USART_InitStructure.USART_BaudRate = BAUD_RATE_LOG;
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
}


void task_1(void * argument) {
  while (1) {
    // Application code
    Toggle_LED_1();
    vTaskDelay(1000);
  }
}

void task_2(void * argument) {
  while (1) {
    // Application code
    Toggle_LED_2();
    vTaskDelay(500);
  }
}

void task_3(void * argument) {
  while (1) {
    // Application code
//		sprintf(board871.board871_log_content, "%u\n", CURRENT_TICK);
//		Write_String_Log(board871.board871_log_content);
		if (!Get_GPS_String(&board871.lc76f)) {
			vTaskDelay(1000);
			continue;
		}
		
		Parse_GPS_String(&board871.lc76f);
    vTaskDelay(1000);
  }
}

void task_4(void * argument) {
  while (1) {
    // Application code
    vTaskDelay(500);
  }
}

int main(void) {
  SystemCoreClockUpdate();
	
	LED_Init();
	Initialize_Log();

  Board871_Initialize( & board871);

  // Create application main thread
  xTaskCreate(task_1, "task_1", 128, NULL, 2, NULL);
  xTaskCreate(task_2, "task_2", 128, NULL, 2, NULL);
  xTaskCreate(task_3, "task_3", 128, NULL, 2, NULL);
  xTaskCreate(task_4, "task_4", 128, NULL, 2, NULL);
	
  // Start the kernel and execute the first thread
  vTaskStartScheduler();

  while (1);
}

//void setup(struct BC660K * self) {	
///* Initialize UART ports */
//  UART0_GNSS_Configuration();
//  USART1_DEBUG_Configuration();

///* Initialize I2C and Acce */
//	I2C_Configuration();
//	MC3416_Init();

///* Initialize BC660K_handler */

//  sprintf(self -> log_content, "Setup successfully!\n");
//  writeLog(self);

//	addCA(self);
//}

//void addCA(struct BC660K * self) {
//	checkModule_AT(self);
//	offEcho_ATE0(self);
//	setAuthentication_AT_QSSLCFG(self);
//	setCACert_AT_QSSLCFG(self);
//	setClientCert_AT_QSSLCFG(self);
//	setClientPrivateKey_AT_QSSLCFG(self);
//	enableSSL_AT_QMTCFG(self);
//}

//void loop(struct BC660K * self) {
//	MC3416_Read_Accel(&Ax, &Ay, &Az);

//	getRawGPS();
//	USART1_Send((char*) data);
//	printBool(getRawGPS());
//	extractMainData();
//	USART1_Send_Float(latitude);
//	USART1_Send_Float(longitude);
//	USART1_Send_Float(calculateDistance());

//	printf("Ax = %d, Ay = %d, Az = %d\r\n", Ax, Ay, Az);
//	USART1_Send_Int16(Ax);
//	USART1_Send_Int16(Ay);
//	USART1_Send_Int16(Az);

//	UART0_Receive();

//	offEcho_ATE0(self);
//	getIMEI_AT_CGSN(self);
//	setAuthentication_AT_QSSLCFG(self);
//	setCACert_AT_QSSLCFG(self);
//	setClientCert_AT_QSSLCFG(self);
//	setClientPrivateKey_AT_QSSLCFG(self);
//	getModelID_AT_CGMM(self);
//	checkNetworkRegister_AT_CEREG(self);
//	getNetworkStatus_AT_QENG(self);
//	checkModule_AT(self);
//	closeMQTT_AT_QMTCLOSE(self);

//	wakeUpModule_AT_QSCLK(self);
//	openMQTT_AT_QMTOPEN(self);
//	connectClient_AT_QMTCONN(self);
//	publishMessage_AT_QMTPUB(self);
//	publishMessage_AT_QMTPUB(self);
//	publishMessage_AT_QMTPUB(self);
//	closeMQTT_AT_QMTCLOSE(self);
//}

//void printBool(bool b) 
//{
//  char buf[6]; // enough to hold "false" or "true"
//  sprintf(buf, "%s\r\n", b ? "Valid\r\n" : "Invalid\r\n");
//  USART1_Send(buf);
//}

//void extractMainData(void)
//{
//	extern uint8_t data[100];
//	extern float latitude;
//	extern float longitude;
//	uint8_t* check = NULL;
//	int b = 0;
//	float a = 0.0;
//	float c = 0.0;
//	
//	check = strtok(data, ",");
//	check = strtok(NULL, ",");
//	check = strtok(NULL, ",");
//	
//	check = strtok(NULL, ",");
//	a = atof(check);
//	b = a/100;
//	c  = a-b*100;
//	latitude = (b + (c/60));
//	check = strtok(NULL, ",");
//	if (strcmp(check, "S") == 0)
//	{
//		latitude = latitude*(-1);
//	}
//	
//	check = strtok(NULL, ",");
//	a = atof(check);
//	b = a/100;
//	c  = a-b*100;
//	longitude = (b + (c/60));
//	check = strtok(NULL, ",");
//	if (strcmp(check, "S") == 0)
//	{
//		longitude = longitude*(-1);
//	}
//}


//float calculateDistance(void)
//{
//	extern float latitude, current_lat;
//	extern float longitude, current_lon;
//	double theta1, theta2, delta_lat, delta_lon;
//	double a,c,d;
//	
//	theta1 = current_lat * PI / 180;
//	theta2 = latitude * PI / 180;
//	
//	delta_lat = (latitude - current_lat) * PI / 360;
//	delta_lon = (longitude - current_lon) * PI / 360;
//	
//	a = sin(delta_lat)*sin(delta_lat) + cos(theta1)*cos(theta2)*sin(delta_lon)*sin(delta_lon);
//	c = asin(sqrt(a));
//	d = 2*6378137*c;
//	
//	return d;
//}

//void packMsg()
//{
//	char buffer[120]; // adjust buffer size as needed
//  sprintf(buffer, "{\"message\":{\"time\":\"15-05-2023 15:11:35\",\"acce_x\":\"%hd\",\"acce_y\":\"%hd\",\"acce_z\":\"%hd\",\"lat\":\"%f\",\"long\":\"%f\"}}\r\n", Ax, Ay, Az, latitude, longitude); // convert float to string with 6 decimal places
//  USART1_Send(buffer); // send string over USART1
//}
