#ifndef MC3416_
#define MC3416_

/* Includes */
#include "ht32.h"

#include "Setting.h"

#include "Collections.h"

/* Extern variables and functions */
extern void Error_Blinking_LED_1(void);
extern void Connecting_Blinking_LED_2(void);

extern void Initialize_Log(void);
extern void Write_Char_Log(u16 character);
extern void Write_String_Log(char * input_string);

/* Struct declaration */
static struct MC3416 {
  char * mc3416_log_content;
	
	struct Accel accel;
}
MC3416;

/* Function prototypes */
void MC3416_Initialize(struct MC3416 * self);

void MC3416_Init(void);
void MC3416_I2C_Configuration(void);
void MC3416_Read_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t * str_data);
void MC3416_Write_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t data);
void MC3416_Read_Accel(struct MC3416 * self, struct Node *current_node);

#endif /* MC3416_ */