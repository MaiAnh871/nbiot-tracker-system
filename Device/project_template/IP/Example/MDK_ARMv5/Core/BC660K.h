#ifndef BC660K_
#define BC660K_

/* Includes */
#include "ht32_board.h"

#include "stdlib.h"

#include "string.h"

#include "SSL_param.h"

#include "Setting.h"

#include "Collections.h"

/* Extern variables and functions */
extern void Error_Blinking_LED_1(void);
extern void Connecting_Blinking_LED_2(void);

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
enum StatusType offEcho_ATE0(struct BC660K *self);
enum StatusType getIMEI_AT_CGSN(struct BC660K *self);
enum StatusType getModelID_AT_CGMM(struct BC660K *self);
enum StatusType checkNetworkRegister_AT_CEREG(struct BC660K *self);
enum StatusType getNetworkStatus_AT_QENG(struct BC660K *self);
enum StatusType setAuthentication_AT_QSSLCFG(struct BC660K *self);
enum StatusType setCACert_AT_QSSLCFG(struct BC660K *self);
enum StatusType setClientCert_AT_QSSLCFG(struct BC660K *self);
enum StatusType setClientPrivateKey_AT_QSSLCFG(struct BC660K *self);
enum StatusType enableSSL_AT_QMTCFG(struct BC660K *self);
enum StatusType openMQTT_AT_QMTOPEN(struct BC660K *self);
enum StatusType connectClient_AT_QMTCONN(struct BC660K *self);
enum StatusType publishMessage_AT_QMTPUB(struct BC660K *self);
enum StatusType closeMQTT_AT_QMTCLOSE(struct BC660K *self);
enum StatusType wakeUpModule_AT_QSCLK(struct BC660K *self);

#endif /* BC660K_ */