#ifndef LC76F_
#define LC76F_

/* Includes */
#include "ht32.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


/* Struct declaration */
struct LC76F {
	char *lc76f_log_content;
} LC76F;


/* Function prototypes */
void LC76F_Initialize(struct LC76F *self);
void UART0_GNSS_Configuration(void);
void UART0_Receive(void);
void UART0_Read_Block(uint8_t  *GPS_data);
void Get_Lat_Lon(void);
void Update_Lat_Lon(void);
float Calculate_Distance(void);
bool Get_GPS_data(void);
bool Check_valid_data(uint8_t  *GPS_data);
extern void clear(uint8_t *input_string);


#endif /* LC76F_ */