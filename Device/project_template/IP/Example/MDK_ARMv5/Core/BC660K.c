#include "BC660K.h"


extern void BC660K_Initialize(struct BC660K * self) {
  BC660K_USART0_Configuration();

  self -> bc660k_log_content = (char * ) malloc(BC660K_LOG_CONTENT_SIZE * sizeof(char));
  if (!self -> bc660k_log_content) {
	Error_Blinking_LED_1();
  }

  self -> command = (char * ) malloc(BC660K_COMMAND_SIZE * sizeof(char));
  if (!self -> command) {
	Error_Blinking_LED_1();
  }

  self -> receive_buffer = (char * ) malloc(BC660K_RECEIVE_BUFFER_SIZE * sizeof(char));
  if (!self -> receive_buffer) {
	Error_Blinking_LED_1();
  }
}

void BC660K_USART0_Configuration(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PA = 1;
    CKCUClock.Bit.USART0 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_3, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_3, AFIO_FUN_USART_UART);

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
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_USART0, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_USART0, ENABLE);
  USART_RxCmd(HT_USART0, ENABLE);
}

void BC660K_USART0_Send_Char(u16 Data) {
  while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXC) == RESET);
  USART_SendData(HT_USART0, Data);
}

void BC660K_USART0_Send(char * input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    BC660K_USART0_Send_Char(input_string[i]);
  }
}

enum StatusType BC660K_USART0_Receive(struct BC660K * self) {
  enum StatusType output_status = STATUS_TIMEOUT;
  u16 uData;
  u8 index;
  char * ptr;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDR) == SET) {
    uData = USART_ReceiveData(HT_USART0);
    self -> receive_buffer[self -> receive_buffer_index] = uData;
    self -> receive_buffer_index++;
  }

  for (index = 0; index < SUCCESS_COMMAND_SIGN_LENGTH; index++) {
    ptr = strstr(self -> receive_buffer, SUCCESS_COMMAND_SIGN[index]);
    if (ptr) {
      output_status = STATUS_SUCCESS;
      return output_status;
    }
  }

  for (index = 0; index < ERROR_COMMAND_SIGN_LENGTH; index++) {
    ptr = strstr(self -> receive_buffer, ERROR_COMMAND_SIGN[index]);
    if (ptr) {
      output_status = STATUS_ERROR;
      return output_status;
    }
  }

  return output_status;
}

enum StatusType BC660K_Send_Command(struct BC660K * self, u8 send_attempt, u32 command_timeout) {
  enum StatusType output_status = STATUS_UNKNOWN;
  if (send_attempt <= 0) {
    send_attempt = BC660K_SEND_ATTEMPT_DEFAULT;
  }
  u8 count = send_attempt;

	char *command;
	command = (char * ) malloc(BC660K_COMMAND_SIZE * sizeof(char));
	if (!command) {
	Error_Blinking_LED_1();
	}

	strcpy(command, self->command);

  while (count--) {

    sprintf(self -> bc660k_log_content, "\n=== SENDING <%s> | ATTEMPT %u/%u ===\n", self -> command, (send_attempt - count), send_attempt);
		Write_String_Log(self -> bc660k_log_content);
	
		BC660K_Clear_Receive_Buffer(self);

    BC660K_USART0_Send(self -> command);
    BC660K_USART0_Send((char * )
      "\r\n");

		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= command_timeout) {
				output_status = BC660K_USART0_Receive(self);
		}

    sprintf(self -> bc660k_log_content, "%s\n\n", self -> receive_buffer);
		Write_String_Log(self -> bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
    sprintf(self -> bc660k_log_content, "Command status: %s\n", getStatusTypeString(output_status));
		Write_String_Log(self -> bc660k_log_content);
    sprintf(self -> bc660k_log_content, "==========\n");
		Write_String_Log(self -> bc660k_log_content);

		vTaskDelay(BC660K_SEND_COMMAND_DELAY_MS);
		
    if (output_status == STATUS_SUCCESS) {
      break;
    }
  }

  return output_status;
}

void BC660K_Clear_Receive_Buffer(struct BC660K * self) {
  for (self -> receive_buffer_index = 0; self -> receive_buffer_index < BC660K_RECEIVE_BUFFER_SIZE; self -> receive_buffer_index++) {
    self -> receive_buffer[self -> receive_buffer_index] = 0;
  }
  self -> receive_buffer_index = 0;
}

enum StatusType checkModule_AT(struct BC660K * self) {
  /* Initialize status */
  enum StatusType output_status = STATUS_UNKNOWN;

  /* Write Command */
  sprintf(self -> command, "AT");
  output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);

  /* Actions with status */
  switch (output_status) {

  case STATUS_SUCCESS:
    /* Do something */
    break;

  case STATUS_ERROR:
    /* Do something */
    break;

  case STATUS_TIMEOUT:
    /* Do something */
    break;

  case STATUS_BAD_PARAMETERS:
    /* Do something */
    break;

  default:
    /* Do something */
    break;
  }

  return output_status;
}

enum StatusType offEcho_ATE0(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "ATE0");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType getIMEI_AT_CGSN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+CGSN");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType getModelID_AT_CGMM(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+CGMM");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType checkNetworkRegister_AT_CEREG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+CEREG?");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType getNetworkStatus_AT_QENG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QENG=0");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setAuthentication_AT_QSSLCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"seclevel\",2");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setCACert_AT_QSSLCFG(struct BC660K *self)  {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"cacert\"");
		sprintf(self->bc660k_log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "|%s|", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		
		sprintf(self->command, CA_CERT);
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");
		delay_ms(100);
		BC660K_BC660K_USART0_Send_Char(26);
	
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s\n\n", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->bc660k_log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->bc660k_log_content, "==========\n");
		writeLog(self);
		
		delay_ms(BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setClientCert_AT_QSSLCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"clientcert\"");
		sprintf(self->bc660k_log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		
		sprintf(self->command, CLIENT_CERT);
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");
		delay_ms(100);
		BC660K_BC660K_USART0_Send_Char(26);
	
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s\n\n", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->bc660k_log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->bc660k_log_content, "==========\n");
		writeLog(self);
		
		delay_ms(BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType setClientPrivateKey_AT_QSSLCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSSLCFG=0,0,\"clientkey\"");
		sprintf(self->bc660k_log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		
		sprintf(self->command, CLIENT_KEY);
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");
		delay_ms(100);
		BC660K_BC660K_USART0_Send_Char(26);
	
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s\n\n", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->bc660k_log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->bc660k_log_content, "==========\n");
		writeLog(self);
		
		delay_ms(BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType enableSSL_AT_QMTCFG(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTCFG=\"ssl\",0,1,0,0");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType openMQTT_AT_QMTOPEN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTOPEN=0,\"a2ht7rbdkt6040-ats.iot.ap-northeast-2.amazonaws.com\",8883");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 9000);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType connectClient_AT_QMTCONN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTCONN=0,\"anhttm8client\"");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 8000);
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}

		return output_status;
}

enum StatusType publishMessage_AT_QMTPUB(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		
		/* Write Command */
		sprintf(self->command, "AT+QMTPUB=0,0,0,0,\"tracker/data\",300");
		sprintf(self->bc660k_log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
		clearModuleBuffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		

		sprintf(self->command, "{\"message\":{\"time\":\"15-05-2023 15:11:35\",\"acce_x\":\"%hd\",\"acce_y\":\"%hd\",\"acce_z\":\"%hd\",\"lat\":\"%f\",\"long\":\"%f\"}}", Ax, Ay, Az, latitude, longitude);
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");
	
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s\n\n", self->receive_buffer);
		writeLog(self);
		clearModuleBuffer(self);
		sprintf(self->bc660k_log_content, "Command status: %s\n", getStatusTypeString(output_status));
		writeLog(self);
		sprintf(self->bc660k_log_content, "==========\n");
		writeLog(self);
		
		delay_ms(BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType closeMQTT_AT_QMTCLOSE(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTCLOSE=0");
		output_status = sendCommand(self, 2, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}

enum StatusType wakeUpModule_AT_QSCLK(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QSCLK=0");
		output_status = sendCommand(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS:
					/* Do something */
					break;

			case STATUS_ERROR:
					/* Do something */
					break;
			
			case STATUS_TIMEOUT:
					/* Do something */
					break;
			
			case STATUS_BAD_PARAMETERS:
					/* Do something */
					break;
			
			default:
					/* Do something */
					break;
		}
		
		return output_status;
}