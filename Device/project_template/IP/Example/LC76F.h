/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Private constants ---------------------------------------------------------------------------------------*/
#define PI 3,141592653589793238462643383279

/* Private function prototypes -----------------------------------------------------------------------------*/
void UART0_GNSS_Configuration(void);
void UART0_Receive(void);
void UxART_Read_Block(uint8_t  *GPS_data);
void Get_Lat_Lon(void);
void Update_Lat_Lon(void);
float Calculate_Distance(void);
bool Get_GPS_data(void);
bool Check_valid_data(uint8_t  *GPS_data);
extern void clear(uint8_t *input_string);