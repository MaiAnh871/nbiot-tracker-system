#ifndef BC660K_
#define BC660K_

/* Includes */
#include "ht32.h"
#include "ht32_board.h"
#include "string.h"

#include "Setting.h"
#include "Collections.h"


/* Struct declaration */
struct BC660K {
	/* Debug */
  char * log_content;
	
	/* Command */
	vu32 command_timer;
	char *command;
	char *module_buffer;
	u16 module_buffer_index;
} BC660K;


/* Function prototypes */
void BC660K_Initialize(struct BC660K *self);

void BC660K_USART0_Configuration(void);
void BC660K_USART0_Send_Char(u16 Data);
void BC660K_USART0_Send(char * input_string);
enum StatusType USART0_Receive(struct BC660K *self);

#endif /* BC660K_ */