#include "Board871.h"

void Board871_Initialize(struct Board871 * self) {
  self -> board871_log_content = (char * ) malloc(BOARD871_LOG_CONTENT * sizeof(char));
  if (!self -> board871_log_content) {
		Error_Blinking_LED_1();
  }
	
	Create_New_Node(self);

  BC660K_Initialize( & self -> bc660k);
  LC76F_Initialize( & self->lc76f);
  MC3416_Initialize( & self->mc3416);
}

void Create_New_Node(struct Board871 * self) {
	self->current_node = malloc (sizeof(Node));
	
	self->current_node->valid = false;
	
	self->current_node->timestamp.hour = 0;
	self->current_node->timestamp.minute = 0;
	self->current_node->timestamp.second = 0;
	self->current_node->timestamp.day = 1;
	self->current_node->timestamp.month = 1;
	self->current_node->timestamp.year = 1970;
	
	sprintf(self->current_node->device_id, "%s", "00-00-00-00-00-00");
	
	self->current_node->latitude.degree = 0;
	self->current_node->latitude.minute = 0;
	self->current_node->latitude.second = 0;
	self->current_node->latitude.latitude_direction = NORTH;
	
	self->current_node->longitude.degree = 0;
	self->current_node->longitude.minute = 0;
	self->current_node->longitude.second = 0;
	self->current_node->longitude.longitude_direction = EAST;
	
	self->current_node->speed = 0;
	
	self->current_node->accel_x = 0;
	self->current_node->accel_y = 0;
	self->current_node->accel_z = 0;
	
	self->current_node->tilt_alert = false;
	self->current_node->wheelie_alert = false;
	self->current_node->overspeed_alert = false;
	
	self->current_node->connection_status.rsrp = 0;
	sprintf(self->current_node->connection_status.cell_id, "%s", "00000000");
	
	self->current_node->next_node = NULL;
}

/* Debug */
void Print_Node(struct Board871 * self, struct Node *input_node) {
	if (!input_node) {
		Write_String_Log("Input node is NULL!\n");
		return;
	}
	
	char *temp;
	temp = calloc(150, sizeof(char));
	 
	sprintf(self->board871_log_content, "{");
	
	sprintf(temp, "\"timestamp\":%u:%u:%uT%u-%u-%u", input_node->timestamp.hour, input_node->timestamp.minute, input_node->timestamp.second, input_node->timestamp.day, input_node->timestamp.month, input_node->timestamp.year);
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"device_id\":\"%s\"", input_node->device_id);
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"latitude\":{\"degree\":%d,\"minute\":%d,\"second\":%d}",input_node->latitude.degree, input_node->latitude.minute, input_node->latitude.second);
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"longtitude\":{\"degree\":%d,\"minute\":%d,\"second\":%d}",input_node->longitude.degree, input_node->longitude.minute, input_node->longitude.second);
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"speed\":%u", (uint16_t) input_node->speed);
	strcat(self->board871_log_content, temp);	

	sprintf(temp, ",\"accel_x\":%d", (int16_t) input_node->accel_x);
	strcat(self->board871_log_content, temp);	

	sprintf(temp, ",\"accel_y\":%d", (int16_t) input_node->accel_x);
	strcat(self->board871_log_content, temp);	

	sprintf(temp, ",\"accel_z\":%d", (int16_t) input_node->accel_x);
	strcat(self->board871_log_content, temp);
	
	if (input_node->tilt_alert) {
		sprintf(temp, ",\"tilt_alert\":true");
	} else {
		sprintf(temp, ",\"tilt_alert\":false");
	}
	strcat(self->board871_log_content, temp);
	
	if (input_node->wheelie_alert) {
		sprintf(temp, ",\"wheelie_alert\":true");
	} else {
		sprintf(temp, ",\"wheelie_alert\":false");
	}
	strcat(self->board871_log_content, temp);
	
	if (input_node->overspeed_alert) {
		sprintf(temp, ",\"overspeed_alert\":true");
	} else {
		sprintf(temp, ",\"overspeed_alert\":false");
	}
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"connection_status\":{\"cell_id\":\"%s\",\"rsrp\":%d}", input_node->connection_status.cell_id, input_node->connection_status.rsrp);
	strcat(self->board871_log_content, temp);
	
	strcat(self->board871_log_content, "}");
		
	Write_String_Log(self->board871_log_content);
	
	free(temp);
}