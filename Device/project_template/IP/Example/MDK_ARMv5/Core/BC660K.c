#include "BC660K.h"


extern void BC660K_Initialize(struct BC660K *self){
	BC660K_USART0_Configuration();
	
  self->bc660k_log_content = (char *) malloc(BC660K_LOG_CONTENT_SIZE * sizeof(char));
  if (!self->bc660k_log_content) {
    Toggle_LED_1();
    while (1);
  }
	
  self->command = (char *) malloc(BC660K_COMMAND_SIZE * sizeof(char));
  if (!self -> command) {
    Toggle_LED_1();
    while (1);
  }
	
  self->receive_buffer = (char *) malloc(BC660K_RECEIVE_BUFFER_SIZE * sizeof(char));
  if (!self -> receive_buffer) {
    Toggle_LED_1();
    while (1);
  }
}


void BC660K_USART0_Configuration(void){
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


void BC660K_USART0_Send(char *input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    BC660K_USART0_Send_Char(input_string[i]);
  }
}


enum StatusType BC660K_USART0_Receive(struct BC660K *self) {
		enum StatusType output_status = STATUS_TIMEOUT;
		u16 uData;
		u8 index;
		char *ptr;

		/* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
		if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDR) == SET) {
			uData = USART_ReceiveData(HT_USART0);
			self->receive_buffer[self->receive_buffer_index] = uData;
			self->receive_buffer_index++;
		}

		for (index = 0; index < SUCCESS_COMMAND_SIGN_LENGTH; index++){
				ptr = strstr(self->receive_buffer, SUCCESS_COMMAND_SIGN[index]);
				if (ptr) {
						output_status = STATUS_SUCCESS;
						return output_status;
				}
		}
		
		for (index = 0; index < ERROR_COMMAND_SIGN_LENGTH; index++){
				ptr = strstr(self->receive_buffer, ERROR_COMMAND_SIGN[index]);
				if (ptr) {
						output_status = STATUS_ERROR;
						return output_status;
				}
		}
		
		return output_status;
}


enum StatusType BC660K_Send_Command(struct BC660K *self, u8 send_attempt, u32 command_timeout) {
		enum StatusType output_status = STATUS_UNKNOWN;
		if (send_attempt <= 0) {
				send_attempt = BC660K_SEND_ATTEMPT_DEFAULT;
		}
		u8 count = send_attempt;
		
//		char *command;
//		command = (char * ) malloc(COMMAND_SIZE * sizeof(char));
//		if (!command) {
//			Toggle_LED_1();
//			while (1);
//		}
		
//		strcpy(command, self->command);
		
		while (count--){
				
				sprintf(self->bc660k_log_content, "\n=== SENDING <%s> | ATTEMPT %u/%u ===\n", self->command, (send_attempt-count), send_attempt);
//				writeLog(self);
//			
//				clearModuleBuffer(self);
				
				
				BC660K_USART0_Send(self->command);
				BC660K_USART0_Send((char *)"\r\n");

//				self->command_timer = portNVIC_SYSTICK_CURRENT_VALUE_REG;
//				while(portNVIC_SYSTICK_CURRENT_VALUE_REG - self->command_timer <= command_timeout) {
////						output_status = USART0_Receive(self);
//				}
				
				sprintf(self->bc660k_log_content, "%s\n\n", self->receive_buffer);
//				writeLog(self);
//				clearModuleBuffer(self);
				sprintf(self->bc660k_log_content, "Command status: %s\n", getStatusTypeString(output_status));
//				writeLog(self);
				sprintf(self->bc660k_log_content, "==========\n");
//				writeLog(self);
				
//				delay_ms(BC660K_SEND_COMMAND_DELAY_MS);
				
				if (output_status == STATUS_SUCCESS) {
						break;
				}
		}
		
		return output_status;
}


void BC660K_Clear_Receive_Buffer(struct BC660K *self) {
		for (self->receive_buffer_index = 0; self->receive_buffer_index < BC660K_RECEIVE_BUFFER_SIZE; self->receive_buffer_index++) {
				self->receive_buffer[self->receive_buffer_index] = 0;
		}
		self->receive_buffer_index = 0;
}


enum StatusType checkModule_AT(struct BC660K *self) {
		/* Initialize status */
		enum StatusType output_status = STATUS_UNKNOWN;
		
		/* Write Command */
		sprintf(self->command, "AT");
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