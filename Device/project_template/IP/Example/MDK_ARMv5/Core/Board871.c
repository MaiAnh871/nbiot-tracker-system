#include "Board871.h"

void Board871_Initialize(struct Board871 * self) {
  self -> board871_log_content = (char * ) malloc(BOARD871_LOG_CONTENT * sizeof(char));
  if (!self -> board871_log_content) {
		Error_Blinking_LED_1();
  }

  BC660K_Initialize( & self -> bc660k);
  LC76F_Initialize( & self->lc76f);
  MC3416_Initialize( & self->mc3416);
}







/* Debug */
void Print_Node(struct Board871 * self, struct Node *input_node) {
	if (!input_node) {
		Write_String_Log("Input node is NULL!\n");
		return;
	}
	
	char temp[100];
	
	sprintf(self->board871_log_content, "{");
	
	if (input_node->valid) {
		sprintf(temp, "\"valid\": true");
	} else {
		sprintf(temp, "\"valid\": true");
	}
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"device_id\": \"%s\"", "mai-anh-cong-chua");
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"latitude\":{\"degree\":%d,\"minute\":%d,\"second\":%d,\"direction\":%d}",input_node->latitude.degree, input_node->latitude.minute, input_node->latitude.second, input_node->latitude.latitude_direction);
	strcat(self->board871_log_content, temp);
}