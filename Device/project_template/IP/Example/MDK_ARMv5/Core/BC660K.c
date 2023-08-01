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
	
	strcpy(self->connection_status.cell_id, "00000000");
	self->connection_status.rsrp = 0;
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

  while (count--) {

    sprintf(self -> bc660k_log_content, "\n=== SENDING <%s> | ATTEMPT %u/%u ===", self -> command, (send_attempt - count), send_attempt);
		Write_String_Log(self -> bc660k_log_content);

		BC660K_Clear_Receive_Buffer(self);
		Write_String_Log(self -> command);
    BC660K_USART0_Send(self -> command);
    BC660K_USART0_Send((char * )
      "\r\n");

		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= command_timeout) {
				output_status = BC660K_USART0_Receive(self);
		}

    sprintf(self -> bc660k_log_content, "%s", self -> receive_buffer);
		Write_String_Log(self -> bc660k_log_content);
    sprintf(self -> bc660k_log_content, "Command status: %s\n", getStatusTypeString(output_status));
		Write_String_Log(self -> bc660k_log_content);
    sprintf(self -> bc660k_log_content, "==========\n");
		Write_String_Log(self -> bc660k_log_content);
		
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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		self->stat = 2;
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS: {
					/* Do something */
					uint8_t token_num;
					char ** token = Tokenize_String(self->receive_buffer, ",", &token_num);
					char *ptr = strstr(token[0], "+CEREG");
					if (ptr && (token_num >= 2)) {
						self->stat = atoi(token[1]);
//						sprintf(self->bc660k_log_content, "STAT: %u", self->stat);
//						Write_String_Log(self->bc660k_log_content);
					}
					break;
			}

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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		// +QENG: 0,1769,9,460,"048FFFCA",-67,-3,-64,13,3,"A794",0,-128,2
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS: {
					/* Do something */
					uint8_t token_num = 0;
//					sprintf(self->bc660k_log_content, "QENG - receive_buffer: %s", self->receive_buffer);
//					Write_String_Log(self->bc660k_log_content);
					char *ptr = strstr(self->receive_buffer, "\"");
					if (ptr) {
//						sprintf(self->bc660k_log_content, "QENG - ptr: %s", ptr);
//						Write_String_Log(self->bc660k_log_content);
						char ** token = Tokenize_String(ptr, ",", &token_num);
						
//						sprintf(self->bc660k_log_content, "QENG - token[0]: %s  |  token[1]: %d", token[0], atoi(token[1]));
//						Write_String_Log(self->bc660k_log_content);
						if (strlen(token[0]) != 10 || atoi(token[1]) > -40) {
							output_status = STATUS_ERROR;
							free(token);
							break;
						}
						
						removeChars(token[0], "\"");
						strcpy(self->connection_status.cell_id, token[0]);
						self->connection_status.rsrp = atoi(token[1]);
						free(token);
					} else {
						output_status = STATUS_ERROR;
					}
					break;
			}

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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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
		sprintf(self->bc660k_log_content, "=== SENDING <%s> ===", self->command);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
//		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = BC660K_USART0_Receive(self);
//		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
		
		vTaskDelay(1);
		sprintf(self->command, CA_CERT);
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");
		vTaskDelay(1);
		BC660K_USART0_Send_Char(26);
	
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = BC660K_USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
		sprintf(self->bc660k_log_content, "Command status: %s", getStatusTypeString(output_status));
		Write_String_Log(self->bc660k_log_content);
		sprintf(self->bc660k_log_content, "==========");
		Write_String_Log(self->bc660k_log_content);
		
		vTaskDelay(BC660K_SEND_COMMAND_DELAY_MS);
		
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
		sprintf(self->bc660k_log_content, "=== SENDING <%s> ===", self->command);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
//		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = BC660K_USART0_Receive(self);
//		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
		
		vTaskDelay(1000);
		sprintf(self->command, CLIENT_CERT);
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");
		vTaskDelay(1);
		BC660K_USART0_Send_Char(26);
	
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = BC660K_USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
		sprintf(self->bc660k_log_content, "Command status: %s", getStatusTypeString(output_status));
		Write_String_Log(self->bc660k_log_content);
		sprintf(self->bc660k_log_content, "==========");
		Write_String_Log(self->bc660k_log_content);
		
		vTaskDelay(BC660K_SEND_COMMAND_DELAY_MS);
		
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
		sprintf(self->bc660k_log_content, "=== SENDING <%s> ===", self->command);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
//		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = BC660K_USART0_Receive(self);
//		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
		
		vTaskDelay(3000);
		sprintf(self->command, CLIENT_KEY);
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");
		vTaskDelay(10);
		BC660K_USART0_Send_Char(26);
	
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000)) {
				output_status = BC660K_USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
		sprintf(self->bc660k_log_content, "Command status: %s", getStatusTypeString(output_status));
		Write_String_Log(self->bc660k_log_content);
		sprintf(self->bc660k_log_content, "==========");
		Write_String_Log(self->bc660k_log_content);
		
		vTaskDelay(BC660K_SEND_COMMAND_DELAY_MS);
		
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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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

enum StatusType configureEDRX_AT_QEDRXCFG(struct BC660K *self, uint8_t mode) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */

		if (mode == 0) {
			sprintf(self->command, "AT+QEDRXCFG=0");
		} else if (mode == 1) {
			sprintf(self->command, "AT+QEDRXCFG=1,5,\"1111\",\"0000\"");
		} else if (mode == 2) {
			sprintf(self->command, "AT+QEDRXCFG=2,5,\"1111\",\"0000\"");
		} else {
			output_status = STATUS_BAD_PARAMETERS;
			return output_status;
		}
			
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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
};
enum StatusType checkEDRX_AT_CEDRXRDP(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+CEDRXRDP");
			
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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
};

enum StatusType checkMQTT_AT_QMTOPEN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTOPEN?");
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		self->mqtt_opened = false;
		switch(output_status){
			
			case STATUS_SUCCESS: {
					/* Do something */
					char *ptr;
					ptr = strstr(self->receive_buffer, "+QMTOPEN");
					if (ptr) {
						self->mqtt_opened = true;
					}
					break;
			}

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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 8000);
	
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS: {
					/* Do something */
					output_status = STATUS_ERROR;
					char *ptr;
					ptr = strstr(self->receive_buffer, "+QMTOPEN");
					if (!ptr) {
					} else {
						uint8_t num_token;
						char ** token = Tokenize_String(self->receive_buffer, ",", &num_token);
						if (num_token >= 1) {
							if (atoi(token[1]) == 0) {
								Write_String_Log("MQTT IS OPENED!\n");
								self->mqtt_opened = true;
								output_status = STATUS_SUCCESS;
							} else {
								Write_String_Log("MQTT IS NOT OPENED!\n");
							}
						}
						free(token);
					}
					break;
			}

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

enum StatusType checkConnectClient_AT_QMTCONN(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT+QMTCONN?");
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
		/* Actions with status */
		self->mqtt_connected = false;
		switch(output_status){
			
			case STATUS_SUCCESS: {
					/* Do something */
					char *ptr;
					ptr = strstr(self->receive_buffer, "+QMTCONN");
					if (ptr) {
						self->mqtt_connected = true;
					}
					break;
			}

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
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS + 2000);
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS: {
					/* Do something */
					char *ptr;
					ptr = strstr(self->receive_buffer, "+QMTCONN");
					if (!ptr) {
						output_status = STATUS_ERROR;
					} 
					break;
			}

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

enum StatusType publishMessage_AT_QMTPUB(struct BC660K *self, char *data) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
	
		if (!data) {
			output_status = STATUS_BAD_PARAMETERS;
			sprintf(self->bc660k_log_content, "Command status: %s", getStatusTypeString(output_status));
			Write_String_Log(self->bc660k_log_content);
			sprintf(self->bc660k_log_content, "==========");
			Write_String_Log(self->bc660k_log_content);
			return output_status;
		}
		
		/* Write Command */
		sprintf(self->command, "AT+QMTPUB=0,0,0,0,\"anhttm8-tracker/861536030196001/message\",%u", strlen(data));
		sprintf(self->bc660k_log_content, "=== SENDING <%s> ===", self->command);
		Write_String_Log(self->bc660k_log_content);
		BC660K_Clear_Receive_Buffer(self);
	
		BC660K_USART0_Send(self->command);
		BC660K_USART0_Send((char *)"\r\n");

		self->command_timer = CURRENT_TICK;
//		while(CURRENT_TICK - self->command_timer <= BC660K_COMMAND_TIMEOUT_DEFAULT_MS) {
//				output_status = BC660K_USART0_Receive(self);
//		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
	
		vTaskDelay(2000);
		BC660K_USART0_Send(data);
		BC660K_USART0_Send((char *)"\r\n");
		vTaskDelay(10);
		BC660K_USART0_Send_Char(26);
		
		Write_String_Log(data);
		
		self->command_timer = CURRENT_TICK;
		while(CURRENT_TICK - self->command_timer <= (BC660K_COMMAND_TIMEOUT_DEFAULT_MS)) {
				output_status = BC660K_USART0_Receive(self);
		}
		
		sprintf(self->bc660k_log_content, "%s", self->receive_buffer);
		Write_String_Log(self->bc660k_log_content);
		sprintf(self->bc660k_log_content, "Command status: %s", getStatusTypeString(output_status));
		Write_String_Log(self->bc660k_log_content);
		sprintf(self->bc660k_log_content, "==========");
		Write_String_Log(self->bc660k_log_content);
		
		vTaskDelay(BC660K_SEND_COMMAND_DELAY_MS);
		
		/* Actions with status */
		switch(output_status){
			
			case STATUS_SUCCESS: {
					/* Do something */
//					char *ptr;
//					ptr = strstr(self->receive_buffer, "+QMTPUB");
//					if (!ptr) {
//						output_status = STATUS_ERROR;
//					} 
					break;
			}

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
		output_status = BC660K_Send_Command(self, 2, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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

enum StatusType configureSleepMode_AT_QSCLK(struct BC660K *self, uint8_t mode) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		if (mode > 2) {
			output_status = STATUS_BAD_PARAMETERS;
			return output_status;
		}
		
		sprintf(self->command, "AT+QSCLK=%u", mode);
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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

enum StatusType powerSavingModeSetting_AT_CPSMS(struct BC660K *self, uint8_t mode) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */

		if (mode == 0) {
			sprintf(self->command, "AT+CPSMS=0");
		} else if (mode == 1) {
			sprintf(self->command, "AT+CPSMS=1,,,\"10100010\",\"00000100\"");
		} else if (mode == 2) {
			sprintf(self->command, "AT+CPSMS=2");
		} else {
			output_status = STATUS_BAD_PARAMETERS;
			return output_status;
		}
			
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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

enum StatusType enableNBIoTRelatedEventReport(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */

		sprintf(self->command, "AT+QNBIOTEVENT=1,1");	
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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

enum StatusType controlReportDeepSleep(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */

		sprintf(self->command, "AT+QCFG=\"dsevent\",1");	
		output_status = BC660K_Send_Command(self, BC660K_SEND_ATTEMPT_DEFAULT, BC660K_COMMAND_TIMEOUT_DEFAULT_MS);
	
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