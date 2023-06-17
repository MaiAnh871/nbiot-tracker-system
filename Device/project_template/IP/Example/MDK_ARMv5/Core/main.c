/* Includes */
#include "ht32.h"
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
//enum StatusType checkModule_AT(struct BC660K *self);
//enum StatusType offEcho_ATE0(struct BC660K *self);
//enum StatusType getIMEI_AT_CGSN(struct BC660K *self);
//enum StatusType getModelID_AT_CGMM(struct BC660K *self);
//enum StatusType checkNetworkRegister_AT_CEREG(struct BC660K *self);
//enum StatusType getNetworkStatus_AT_QENG(struct BC660K *self);
//enum StatusType setAuthentication_AT_QSSLCFG(struct BC660K *self);
//enum StatusType setCACert_AT_QSSLCFG(struct BC660K *self);
//enum StatusType setClientCert_AT_QSSLCFG(struct BC660K *self);
//enum StatusType setClientPrivateKey_AT_QSSLCFG(struct BC660K *self);
//enum StatusType enableSSL_AT_QMTCFG(struct BC660K *self);
//enum StatusType openMQTT_AT_QMTOPEN(struct BC660K *self);
//enum StatusType connectClient_AT_QMTCONN(struct BC660K *self);
//enum StatusType publishMessage_AT_QMTPUB(struct BC660K *self);
//enum StatusType closeMQTT_AT_QMTCLOSE(struct BC660K *self);
//enum StatusType wakeUpModule_AT_QSCLK(struct BC660K *self);

/* ==================== */

/* UART ports */
void UART0_GNSS_Configuration(void);
void UART0_Receive(void);
void UART0_Read_Block(uint8_t* data);


/* Private macro -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/
struct Board871 board871;

vu32 utick;
uint8_t data[100];
uint8_t* check = NULL;
uint8_t GPS_raw[100];
float latitude;
float longitude;
float current_lat;
float current_lon;
int16_t Ax = 0;
int16_t Ay = 0;
int16_t Az = 0;

/* Global functions ----------------------------------------------------------------------------------------*/
void clear(uint8_t *input_string);
bool getRawGPS(void);
bool checkValidGPS(uint8_t *raw_GPS);
void printBool(bool b);
void extractMainData(void);
void USART1_Send_Float(float f);
void updatePosition(void);
float calculateDistance(void);
void packMsg(void);
void USART1_Send_Int16(int16_t value);
void task_1 (void *argument);
void task_2 (void *argument);
void task_3 (void *argument);
void task_4 (void *argument);
/********************************************************************************************************/
/*
 * @brief  Main program.
 * @retval None
 ***********************************************************************************************************/
//int main(void) {
////  setup(&BC660K_h_h);
//	LED_Init();

//  while (1) {
////    loop(&BC660K_h_h);
//		Toggle_LED_1();
//		Toggle_LED_2();
//		delay_ms(100);
//  }
//}

/*
  Application main thread: Initialize and start the application
*/
void task_1 (void *argument) {
  while(1) {
    // Application code
    Toggle_LED_1();
		vTaskDelay(1000);
  }
}

void task_2 (void *argument) {
  while(1) {
    // Application code
    Toggle_LED_2();
		vTaskDelay(500);
  }
}

void task_3 (void *argument) {
  while(1) {
    // Application code
		vTaskDelay(100);
  }
}

void task_4 (void *argument) {
  while(1) {
    // Application code
		vTaskDelay(100);
  }
}
/*
  Main function: Initialize and start the kernel
*/
int main (void) {
  SystemCoreClockUpdate();
	
	Board871_Initialize(&board871);
	
  // Create application main thread
  xTaskCreate (task_1, "task_1", 16, NULL, 2, NULL);
	xTaskCreate (task_2, "task_2", 16, NULL, 2, NULL);
	xTaskCreate (task_3, "task_3", 16, NULL, 2, NULL);
	xTaskCreate (task_4, "task_4", 16, NULL, 2, NULL);
	
  // Start the kernel and execute the first thread
  vTaskStartScheduler();
 
  while(1);
}

/********************************************************************************************************/
/*
 * @brief  Main program.
 * @retval None
 ***********************************************************************************************************/
//void setup(struct BC660K * self) {	
  /* Initialize UART ports */
//  UART0_GNSS_Configuration();
//  USART0_MODULE_Configuration();
//  USART1_DEBUG_Configuration();

	/* Initialize I2C and Acce */
//	I2C_Configuration();
//	MC3416_Init();

  /* Initialize BC660K_handler */

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
		
//		offEcho_ATE0(self);
//		getIMEI_AT_CGSN(self);
//		setAuthentication_AT_QSSLCFG(self);
//		setCACert_AT_QSSLCFG(self);
//		setClientCert_AT_QSSLCFG(self);
//		setClientPrivateKey_AT_QSSLCFG(self);
//		getModelID_AT_CGMM(self);
//		checkNetworkRegister_AT_CEREG(self);
//		getNetworkStatus_AT_QENG(self);
//		checkModule_AT(self);
//		closeMQTT_AT_QMTCLOSE(self);

//	wakeUpModule_AT_QSCLK(self);
//	openMQTT_AT_QMTOPEN(self);
//	connectClient_AT_QMTCONN(self);
//	publishMessage_AT_QMTPUB(self);
//	publishMessage_AT_QMTPUB(self);
//	publishMessage_AT_QMTPUB(self);
//	closeMQTT_AT_QMTCLOSE(self);
//}

//enum StatusType checkModule_AT(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType wakeUpModule_AT_QSCLK(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QSCLK=0");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType offEcho_ATE0(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "ATE0");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType getIMEI_AT_CGSN(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+CGSN");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType getModelID_AT_CGMM(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+CGMM");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType checkNetworkRegister_AT_CEREG(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+CEREG?");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType getNetworkStatus_AT_QENG(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QENG=0");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType openMQTT_AT_QMTOPEN(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QMTOPEN=0,\"a2ht7rbdkt6040-ats.iot.ap-northeast-2.amazonaws.com\",8883");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS + 9000);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}
//enum StatusType connectClient_AT_QMTCONN(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QMTCONN=0,\"anhttm8client\"");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS + 8000);
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}

//		return output_status;
//}

//enum StatusType publishMessage_AT_QMTPUB(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QMTPUB=0,0,0,0,\"tracker/data\",300");
//		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
//		writeLog(self);
//		clearModuleBuffer(self);
//	
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");

//		self->command_timer = utick;
//		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "%s", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		

//		sprintf(self->command, "{\"message\":{\"time\":\"15-05-2023 15:11:35\",\"acce_x\":\"%hd\",\"acce_y\":\"%hd\",\"acce_z\":\"%hd\",\"lat\":\"%f\",\"long\":\"%f\"}}", Ax, Ay, Az, latitude, longitude);
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");
//	
//		self->command_timer = utick;
//		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "%s\n\n", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
//		writeLog(self);
//		sprintf(self->log_content, "==========\n");
//		writeLog(self);
//		
//		delay_ms(SEND_COMMAND_DELAY_MS);
//		
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType closeMQTT_AT_QMTCLOSE(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QMTCLOSE=0");
//		output_status = sendCommand(self, 2, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType setAuthentication_AT_QSSLCFG(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QSSLCFG=0,0,\"seclevel\",2");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType setCACert_AT_QSSLCFG(struct BC660K *self)  {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QSSLCFG=0,0,\"cacert\"");
//		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
//		writeLog(self);
//		clearModuleBuffer(self);
//	
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");

//		self->command_timer = utick;
//		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "|%s|", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		
//		sprintf(self->command, CA_CERT);
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");
//		delay_ms(100);
//		BC660K_BC660K_USART0_Send_Char(26);
//	
//		self->command_timer = utick;
//		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "%s\n\n", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
//		writeLog(self);
//		sprintf(self->log_content, "==========\n");
//		writeLog(self);
//		
//		delay_ms(SEND_COMMAND_DELAY_MS);
//		
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType setClientCert_AT_QSSLCFG(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QSSLCFG=0,0,\"clientcert\"");
//		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
//		writeLog(self);
//		clearModuleBuffer(self);
//	
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");

//		self->command_timer = utick;
//		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "%s", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		
//		sprintf(self->command, CLIENT_CERT);
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");
//		delay_ms(100);
//		BC660K_BC660K_USART0_Send_Char(26);
//	
//		self->command_timer = utick;
//		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "%s\n\n", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
//		writeLog(self);
//		sprintf(self->log_content, "==========\n");
//		writeLog(self);
//		
//		delay_ms(SEND_COMMAND_DELAY_MS);
//		
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType setClientPrivateKey_AT_QSSLCFG(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QSSLCFG=0,0,\"clientkey\"");
//		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
//		writeLog(self);
//		clearModuleBuffer(self);
//	
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");

//		self->command_timer = utick;
//		while(utick - self->command_timer <= COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "%s", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		
//		sprintf(self->command, CLIENT_KEY);
//		BC660K_USART0_Send(self->command);
//		BC660K_USART0_Send((char *)"\r\n");
//		delay_ms(100);
//		BC660K_BC660K_USART0_Send_Char(26);
//	
//		self->command_timer = utick;
//		while(utick - self->command_timer <= (COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
//				output_status = USART0_Receive(self);
//		}
//		
//		sprintf(self->log_content, "%s\n\n", self->receive_buffer);
//		writeLog(self);
//		clearModuleBuffer(self);
//		sprintf(self->log_content, "Command status: %s\n", getStatusTypeString(output_status));
//		writeLog(self);
//		sprintf(self->log_content, "==========\n");
//		writeLog(self);
//		
//		delay_ms(SEND_COMMAND_DELAY_MS);
//		
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}

//enum StatusType enableSSL_AT_QMTCFG(struct BC660K *self) {
//		/* Initialize status */
//		enum StatusType output_status = STATUS_UNKNOWN;
//		
//		/* Write Command */
//		sprintf(self->command, "AT+QMTCFG=\"ssl\",0,1,0,0");
//		output_status = sendCommand(self, SEND_ATTEMPT_DEFAULT, COMMAND_TIMEOUT_DEFAULT_MS);
//	
//		/* Actions with status */
//		switch(output_status){
//			
//			case STATUS_SUCCESS:
//					/* Do something */
//					break;

//			case STATUS_ERROR:
//					/* Do something */
//					break;
//			
//			case STATUS_TIMEOUT:
//					/* Do something */
//					break;
//			
//			case STATUS_BAD_PARAMETERS:
//					/* Do something */
//					break;
//			
//			default:
//					/* Do something */
//					break;
//		}
//		
//		return output_status;
//}


///* Debug */
//void writeLog(struct BC660K * self) {
//  USART1_Send(self -> log_content);
//}

///*************************************************************************************************************
// * @brief  Configure the UART0 for GNSS
// * @retval None
// ***********************************************************************************************************/
//void UART0_GNSS_Configuration(void) {
//  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

//  {
//    /* Enable peripheral clock of AFIO, UxART                                                                 */
//    CKCUClock.Bit.AFIO = 1;
//    CKCUClock.Bit.PB = 1;
//    CKCUClock.Bit.UART0 = 1;
//    CKCU_PeripClockConfig(CKCUClock, ENABLE);
//  }

//  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
//  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_8, GPIO_PR_UP);

//  /* Config AFIO mode as UxART function.                                                                    */
//  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_7, AFIO_FUN_USART_UART);
//  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_8, AFIO_FUN_USART_UART);

//  {
//    /* UxART configured as follow:
//          - BaudRate = 115200 baud
//          - Word Length = 8 Bits
//          - One Stop Bit
//          - None parity bit
//    */

//    /* !!! NOTICE !!!
//       Notice that the local variable (structure) did not have an initial value.
//       Please confirm that there are no missing members in the parameter settings below in this function.
//    */
//    USART_InitTypeDef USART_InitStructure = {
//      0
//    };
//    USART_InitStructure.USART_BaudRate = 9600;
//    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
//    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
//    USART_InitStructure.USART_Parity = USART_PARITY_NO;
//    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
//    USART_Init(HT_UART0, & USART_InitStructure);
//  }

//  /* Enable UxART Tx and Rx function                                                                        */
//  USART_TxCmd(HT_UART0, ENABLE);
//  USART_RxCmd(HT_UART0, ENABLE);
//}

///*************************************************************************************************************
// * @brief  Configure the USART0
// * @retval None
// ***********************************************************************************************************/

///********************************************************************************************************/
///*
// * @brief  Configure the USART1
// * @retval None
// ***********************************************************************************************************/

///********************************************************************************************************/
///*
// * @brief  UxART Tx Test.
// * @retval None
// ***********************************************************************************************************/

//void UART0_Receive(void) {
//  u16 uData;

//  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
//  if (USART_GetFlagStatus(HT_UART0, USART_FLAG_RXDR) == SET) {
//    uData = USART_ReceiveData(HT_UART0);

//    #if 1 // Loop back Rx data to Tx for test
//    USART1_Send_Char(uData);
//    #endif
//  }
//}

//void UART0_Read_Block(uint8_t  *data)
//{
//	uint8_t index = 0;
//	
//	do
//	{
//  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
//  while (USART_GetFlagStatus(HT_UART0, USART_FLAG_RXDR) == RESET);
//	data[index] = (uint8_t)USART_ReceiveData(HT_UART0);
//	}
//	while ((data[index] != 0x0A) && (index++ != 99));
//}

//void clear(uint8_t *input_string)
//{
//	uint16_t count = 0;
//	for (count = 0; count < 100; count++)
//	{
//		input_string[count] = 0;
//	}
//}

//bool getRawGPS(void)
//{
//	extern uint8_t data[100];
//	uint8_t* check = NULL;
//	uint8_t GPS_raw[100];
//	
//	while (check == NULL)
//	{
//		clear(GPS_raw);
//		UART0_Read_Block(GPS_raw);
//		check = strstr(GPS_raw, "$GNRMC");
//	}
//	strcpy(data, GPS_raw);
//	return checkValidGPS(GPS_raw);
//}

//bool checkValidGPS(uint8_t *raw_GPS)
//{
//	bool valid = false;
//	uint8_t* check = NULL;
//	
//	check = strtok(GPS_raw, ",");
//	check = strtok(NULL, ",");
//	check = strtok(NULL, ",");
//	
//	if (strcmp(check, "A") == 0)
//	{
//		valid = true;
//	}
//	
//	return valid;
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

//void USART1_Send_Float(float f) {
//  char buffer[16]; // adjust buffer size as needed
//  sprintf(buffer, "%.6f\r\n", f); // convert float to string with 6 decimal places
//  USART1_Send(buffer); // send string over USART1
//}

//void updatePosition(void)
//{
//	extern float latitude, current_lat;
//	extern float longitude, current_lon;
//	current_lat = latitude;
//	current_lon = longitude;
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

//enum StatusType USART0_Receive(struct BC660K *self) {
//		enum StatusType output_status = STATUS_TIMEOUT;
//		u16 uData;
//		u8 index;
//		char *ptr;

//		/* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
//		if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDR) == SET) {
//			uData = USART_ReceiveData(HT_USART0);
//			self->receive_buffer[self->receive_buffer_index] = uData;
//			self->receive_buffer_index++;
//		}

//		for (index = 0; index < SUCCESS_COMMAND_SIGN_LENGTH; index++){
//				ptr = strstr(self->receive_buffer, SUCCESS_COMMAND_SIGN[index]);
//				if (ptr) {
//						output_status = STATUS_SUCCESS;
//						return output_status;
//				}
//		}
//		
//		for (index = 0; index < ERROR_COMMAND_SIGN_LENGTH; index++){
//				ptr = strstr(self->receive_buffer, ERROR_COMMAND_SIGN[index]);
//				if (ptr) {
//						output_status = STATUS_ERROR;
//						return output_status;
//				}
//		}
//		
//		return output_status;
//}

//void USART1_Send_Int16(int16_t value) {
//  char buffer[6]; // adjust buffer size as needed
//  sprintf(buffer, "%d\r\n", value); // convert uint16_t to string
//  USART1_Send(buffer); // send string over USART1
//}
