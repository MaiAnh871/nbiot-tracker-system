#include "BC660K.h"

void BC660K_Initialize(struct BC660K *self){
	BC660K_USART0_Configuration();
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

void BC660K_USART0_Send(char * input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    BC660K_BC660K_USART0_Send_Char(input_string[i]);
  }
}

enum StatusType USART0_Receive(struct BC660K *self) {
		enum StatusType output_status = STATUS_TIMEOUT;
		u16 uData;
		u8 index;
		char *ptr;

		/* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
		if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDR) == SET) {
			uData = USART_ReceiveData(HT_USART0);
			self->module_buffer[self->module_buffer_index] = uData;
			self->module_buffer_index++;
		}

		for (index = 0; index < SUCCESS_COMMAND_SIGN_LENGTH; index++){
				ptr = strstr(self->module_buffer, SUCCESS_COMMAND_SIGN[index]);
				if (ptr) {
						output_status = STATUS_SUCCESS;
						return output_status;
				}
		}
		
		for (index = 0; index < ERROR_COMMAND_SIGN_LENGTH; index++){
				ptr = strstr(self->module_buffer, ERROR_COMMAND_SIGN[index]);
				if (ptr) {
						output_status = STATUS_ERROR;
						return output_status;
				}
		}
		
		return output_status;
}