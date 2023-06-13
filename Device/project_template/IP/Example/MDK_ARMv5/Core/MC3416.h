#ifndef MC3416_
#define MC3416_

/* Includes */
#include "ht32.h"


/* Struct declaration */
struct MC3416 {
	
} MC3416;


/* Function prototypes */
void MC3416_initialize(struct MC3416 *self);
void I2C_Configuration(void);
void Read_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t* str_data);
void Write_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t data);
void MC3416_Init(void);
void MC3416_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az); 

#endif /* MC3416_ */