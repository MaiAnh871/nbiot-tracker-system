#ifndef BC660K_
#define BC660K_

/* Includes */
#include "ht32.h"
#include "ht32_board.h"


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

#endif /* BC660K_ */