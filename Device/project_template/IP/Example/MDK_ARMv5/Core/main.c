/* Includes */ 
#include "ht32.h"

#include "ht32_board.h"

#include "stdlib.h"

#include "string.h"

#include <math.h>

#include "FreeRTOS.h"

#include "task.h"

#include "Board871.h"

/* Private function prototypes -----------------------------------------------------------------------------*/

/* UART ports */
void UART0_GNSS_Configuration(void);
void UART0_Receive(void);
void UART0_Read_Block(uint8_t * data);

/* Private macro -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/
xTaskHandle TaskHandle_1;
xTaskHandle TaskHandle_2;
xTaskHandle TaskHandle_3;
xTaskHandle TaskHandle_4;

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
void On_LED_1(void);
void Toggle_LED_2(void);
void On_LED_2(void);

void Error_Blinking_LED_1(void);
void Connecting_Blinking_LED_2(void);
void Initialize_Log(void);
void Write_Char_Log(u16 character);
void Write_String_Log(char * input_string);

void task_1(void * argument);
void task_2(void * argument);
void task_3(void * argument);
void task_4(void * argument);

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

void On_LED_1() {
	HT32F_DVB_LEDOn(HT_LED1);
}

void Toggle_LED_2() {
  HT32F_DVB_LEDToggle(HT_LED2);
}

void On_LED_2() {
	HT32F_DVB_LEDOn(HT_LED2);
}

void Error_Blinking_LED_1(void) {
	while (1) {
		Toggle_LED_1();
		vTaskDelay(100);
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
	Write_Char_Log('\n');
}

void task_1(void * argument) {
  while (1) {
		uint32_t start_time;
		uint32_t end_time;
		
		start_time = CURRENT_TICK;

		/* Application start */
		Get_GPS_Data(&board871);
		vTaskDelay(10);
		
		/* Application end */
		
		end_time = CURRENT_TICK;
		
//		sprintf(board871.board871_log_content, "Task 1 period: %u s", (end_time - start_time));
//		Write_String_Log(board871.board871_log_content);
  }
}

void task_2(void * argument) {
  while (1) {
		uint32_t start_time;
		uint32_t end_time;
		
		start_time = CURRENT_TICK;

		/* Application start */
		
		Get_Accel_Data(&board871);
		
		/* Application end */
		
		end_time = CURRENT_TICK;
		
//		sprintf(board871.board871_log_content, "Task 2 period: %u s", (end_time - start_time));
//		Write_String_Log(board871.board871_log_content);
  }
}

void task_3(void * argument) {
  while (1) {
		uint32_t start_time;
		uint32_t end_time;
		
		start_time = CURRENT_TICK;

		/* Application start */
		
		Validate_Node(&board871);
		vTaskDelay(VALIDATE_PERIOD);
		
		/* Application end */
		
		end_time = CURRENT_TICK;
		
//		sprintf(board871.board871_log_content, "Task 3 period: %u s", (end_time - start_time));
//		Write_String_Log(board871.board871_log_content);
  }
}

void task_4(void * argument) {
  while (1) {
		uint32_t start_time;
		uint32_t end_time;
		
		start_time = CURRENT_TICK;

		/* Application start */

		Connection_Flow(&board871);

		/* Application end */
		
		end_time = CURRENT_TICK;
		
//		sprintf(board871.board871_log_content, "Task 4 period: %u s", (end_time - start_time));
//		Write_String_Log(board871.board871_log_content);
  }
}

int main(void) {
  SystemCoreClockUpdate();
	
	LED_Init();
	Initialize_Log();

  Board871_Initialize( & board871);
	
//	Print_Node(&board871, board871.current_node);

  // Create application main thread
  xTaskCreate(task_1, "task_1", 512, NULL, 2, &TaskHandle_1);
  xTaskCreate(task_2, "task_2", 512, NULL, 2, &TaskHandle_2);
  xTaskCreate(task_3, "task_3", 512, NULL, 3, &TaskHandle_3);
  xTaskCreate(task_4, "task_4", 512, NULL, 3, &TaskHandle_4);

  // Start the kernel and execute the first thread
  vTaskStartScheduler();

  while (1);
}
