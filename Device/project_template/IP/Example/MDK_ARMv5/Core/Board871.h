#ifndef BOARD871_
#define BOARD871_

/* Includes */
#include "ht32.h"

#include "ht32_board.h"

#include "stdlib.h"

#include "string.h"

#include "FreeRTOS.h"

#include "task.h"

#include "time.h"

#include "Setting.h"

#include "Collections.h"

#include "Utilities.h"

#include "BC660K.h"

#include "LC76F.h"

#include "MC3416.h"


/* Extern variables and functions */
extern xTaskHandle TaskHandle_1;
extern xTaskHandle TaskHandle_2;
extern xTaskHandle TaskHandle_3;
extern xTaskHandle TaskHandle_4;

extern void Error_Blinking_LED_1(void);
extern void Connecting_Blinking_LED_2(void);

extern void Initialize_Log(void);
extern void Write_Char_Log(u16 character);
extern void Write_String_Log(char * input_string);

/* Struct initialization */
static struct Board871 {
	/* Debug */
	uint8_t stage;
	uint8_t slow;
  char * board871_log_content;
	struct Route route;
	struct Node *previous_node;
	struct Node *current_node;
	struct Node *publishing_node;

  struct BC660K bc660k;
  struct LC76F lc76f;
  struct MC3416 mc3416;
	
	char * temp;
	char * data_string;
	struct Connection_Status connection_status;
	
	bool has_gps;
	bool has_nbiot;
}
Board871;

/* Function prototypes */
void Board871_Initialize(struct Board871 * self);

/* Tasks */
void Suspend_Measuring(struct Board871 *self);
void Resume_Measuring(struct Board871 *self);

/* Debug */
void Print_Node(struct Board871 * self, struct Node *input_node);

/* Main */
void Create_New_Node(struct Board871 * self);
void Validate_Node(struct Board871 *self);
void Add_Node(struct Board871 *self, struct Node *input_node);

void Clear_String_Data(struct Board871 * self);
void Get_GPS_Data(struct Board871 * self);
void Get_Accel_Data(struct Board871 * self);
void Pack_Node_Data(struct Board871 * self, struct Node *input_node);

void Connection_Flow(struct Board871 *self);

#endif /* BOARD871_ */