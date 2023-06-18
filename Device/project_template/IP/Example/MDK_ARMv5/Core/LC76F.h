#ifndef LC76F_
#define LC76F_

/* Includes */
#include "ht32.h"

#include <stdlib.h>

#include <string.h>

#include <math.h>

/* Extern variables and functions */
extern void LED_Init(void);
extern void Toggle_LED_1(void);
extern void Toggle_LED_2(void);

extern void Initialize_Log(void);
extern void Write_Char_Log(u16 character);
extern void Write_String_Log(char * input_string);

/* Struct declaration */
static struct LC76F {
  char * lc76f_log_content;
}
LC76F;

/* Function prototypes */
extern void LC76F_Initialize(struct LC76F * self);

void LC76F_UART0_Configuration(void);
void LC76F_UART0_Receive(void);
void LC76F_UART0_Read_Block(uint8_t  *data);

//void getRawGPS(uint8_t * GPS_data);
//void Get_Lat_Lon(void);
//void Update_Lat_Lon(void);
//float Calculate_Distance(void);
//bool Get_GPS_data(void);
//bool Check_valid_data(uint8_t * GPS_data);
//void clear(uint8_t * input_string);

#endif /* LC76F_ */