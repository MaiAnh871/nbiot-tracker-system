/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"

/* Private constants ---------------------------------------------------------------------------------------*/
#define MC3416_ADDR 0x4C

/* Private function prototypes -----------------------------------------------------------------------------*/
void I2C_Configuration(void);
void Read_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t* data);
void Write_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t data);
void MC3416_Init(void);
void MC3416_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az); 
