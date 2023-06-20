#ifndef LC76F_
#define LC76F_

/* Includes */
#include "ht32.h"

#include <stdlib.h>

#include <string.h>

#include <math.h>

#include "Setting.h"

/* Extern variables and functions */
extern void Error_Blinking_LED_1(void);
extern void Connecting_Blinking_LED_2(void);

extern void Initialize_Log(void);
extern void Write_Char_Log(u16 character);
extern void Write_String_Log(char * input_string);

/* Struct declaration */
static struct LC76F {
  char * lc76f_log_content;
	char * raw_gps_string;
	char * gps_string;
}
LC76F;

/* Function prototypes */
void LC76F_Initialize(struct LC76F * self);

void LC76F_UART0_Configuration(void);
void LC76F_UART0_Receive(void);
void LC76F_UART0_Read_Block(char *data);
void Clear_Raw_GPS_String(struct LC76F * self);
void Clear_GPS_String(struct LC76F * self);

bool Get_GPS_String(struct LC76F * self);
bool Check_Valid_GPS_String(char * gps_raw_string);
//float Calculate_Distance(void);
//bool Get_GPS_data(void);
//bool Check_valid_data(uint8_t * GPS_data);
//void clear(uint8_t * input_string);

#endif /* LC76F_ */