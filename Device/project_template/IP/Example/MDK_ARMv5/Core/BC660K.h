#ifndef BC660K_
#define BC660K_

/* Includes */
#include "ht32_board.h"

#include "stdlib.h"

#include "string.h"

#include "Setting.h"

#include "Collections.h"

/* Extern variables and functions */
extern void LED_Init(void);
extern void Toggle_LED_1(void);
extern void Toggle_LED_2(void);

extern void Initialize_Log(void);
extern void Write_Char_Log(u16 character);
extern void Write_String_Log(char * input_string);

/* Struct declaration */
static struct BC660K {
  /* Debug */
  char * bc660k_log_content;

  /* Command */
  vu32 command_timer;
  char * command;
  char * receive_buffer;
  u16 receive_buffer_index;
}
BC660K;

/* Function prototypes */
extern void BC660K_Initialize(struct BC660K * self);

void BC660K_USART0_Configuration(void);
void BC660K_USART0_Send_Char(u16 Data);
void BC660K_USART0_Send(char * input_string);
enum StatusType BC660K_USART0_Receive(struct BC660K * self);
enum StatusType BC660K_Send_Command(struct BC660K * self, u8 send_attempt, u32 command_timeout);
void BC660K_Clear_Receive_Buffer(struct BC660K * self);

enum StatusType checkModule_AT(struct BC660K * self);

#endif /* BC660K_ */