#ifndef BOARD871_
#define BOARD871_

/* Includes */
#include "ht32.h"
#include "ht32_board.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "BC660K.h"
#include "LC76F.h"
#include "MC3416.h"


/* Struct initialization */
struct Board871 {
	struct BC660K bc660k;
	struct LC76F lc76f;
	struct MC3416 mc3416;
} Board871;


/* Function prototypes */
void LEDInit(void);
void toggleLED1(void);
void toggleLED2(void);
void toggleLED3(void);

void setupBoard(struct Board871 *self);

#endif /* BOARD871_ */