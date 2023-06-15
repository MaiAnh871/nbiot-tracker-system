#include "Board871.h"

void LED_Init() {
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
  HT32F_DVB_LEDInit(HT_LED3);
  HT32F_DVB_LEDOff(HT_LED1);
  HT32F_DVB_LEDOff(HT_LED2);
  HT32F_DVB_LEDOff(HT_LED3);
}

void Toggle_LED_1() {
	HT32F_DVB_LEDToggle(HT_LED1);
}

void Toggle_LED_2() {
	HT32F_DVB_LEDToggle(HT_LED2);
}

void Toggle_LED_3() {
	HT32F_DVB_LEDToggle(HT_LED3);
}

void Board871_Initialize(struct Board871 *self){
	LED_Init();
	
  self->board871_log_content = (char *) malloc(BOARD871_LOG_CONTENT * sizeof(char));
  if (!self->board871_log_content) {
    Toggle_LED_1();
    while (1);
  }
	
	BC660K_Initialize(&self->bc660k);
	LC76F_Initialize(&self->lc76f);
	MC3416_Initialize(&self->mc3416);
}