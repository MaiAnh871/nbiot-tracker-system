#ifndef LC76F_
#define LC76F_

/* Includes */
#include "ht32.h"

#include <stdlib.h>

#include <string.h>

#include <math.h>

#include "time.h"

#include "Setting.h"

#include "Collections.h"

#include "Utilities.h"

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
	char * temp;
	
	bool update_previous_coordinates;
	struct Coordinates previous_coordinates;
	struct Coordinates current_coordinates;
}
LC76F;

/* Function prototypes */
void LC76F_Initialize(struct LC76F * self);

void LC76F_UART0_Configuration(void);
void LC76F_UART0_Receive(void);
void LC76F_UART0_Read_Block(char *data);

void Clear_Raw_GPS_String(struct LC76F * self);
void Clear_GPS_String(struct LC76F * self);
bool Check_Valid_GPS_String(struct LC76F * self);
void Clear_Temp(struct LC76F * self);
bool Get_GPS_String(struct LC76F * self);
void Parse_GPS_String(struct LC76F * self, struct Node *current_node);

float DMS_To_Decimal(uint8_t degree, uint8_t minute, uint16_t second, int8_t sign);
float Degree_To_Rad(float degree);
float Calculate_Distance(struct LC76F * self);
uint32_t Calculate_Time(struct LC76F * self);
float Calculate_Speed(struct LC76F * self);

#endif /* LC76F_ */