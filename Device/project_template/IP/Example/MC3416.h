/* Settings ------------------------------------------------------------------------------------------------*/
#define HTCFG_I2C_MASTER_CLK(CKCUClock) CKCUClock.Bit.I2C0
#define HTCFG_I2C_MASTER_SCL_GPIO_ID    GPIO_PB
#define HTCFG_I2C_MASTER_SCL_AFIO_PIN   AFIO_PIN_1
#define HTCFG_I2C_MASTER_SDA_GPIO_ID    GPIO_PB
#define HTCFG_I2C_MASTER_SDA_AFIO_PIN   AFIO_PIN_0
#define I2C_MASTER_ADDRESS							0
#define ClockSpeed											100000
#define HTCFG_I2C_MASTER_PORT						HT_I2C0

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"

/* Private constants ---------------------------------------------------------------------------------------*/
#define MC3416_ADDR 0x4C

/* Private function prototypes -----------------------------------------------------------------------------*/
void I2C_Configuration(void);
void Read_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t* str_data);
void Write_Mem_Slave(I2C_AddressTypeDef Slave_Adr, uint8_t RegAddr, uint8_t data);
void MC3416_Init(void);
void MC3416_Read_Accel(int16_t *Ax, int16_t *Ay, int16_t *Az); 
