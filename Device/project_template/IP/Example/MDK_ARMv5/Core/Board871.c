#include "Board871.h"

void Board871_Initialize(struct Board871 * self) {
  self -> board871_log_content = (char * ) malloc(BOARD871_LOG_CONTENT * sizeof(char));
  if (!self -> board871_log_content) {
		Error_Blinking_LED_1();
  }
	
	Create_New_Node(self);

  BC660K_Initialize(&self->bc660k);
  LC76F_Initialize(&self->lc76f);
  MC3416_Initialize(&self->mc3416);
	
	self->measure = true;
	self->slow = 0;
	self->route.total_length = 0;
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

void Validate_Node(struct Board871 *self) {
	self->measure = false;
	vTaskDelay(10);
	
	if (!self->current_node) {
		Create_New_Node(self);
		self->measure = true;
		Write_String_Log("Current Node is Null!");
		return;
	}
	
	if (!self->current_node->valid) {
		self->measure = true;
		return;
	}
	
	if (!self->previous_node) {
		self->previous_node = self->current_node;
		Add_Node(self, self->previous_node);
		Create_New_Node(self);
		self->measure = true;
		return;
	}
	
	/* Calculate speed then decide */
	float distance = Calculate_Distance(self);
	uint32_t time_interval = Calculate_Time(self);
	
	if (time_interval == 0) {
		self->measure = true;
		return;
	}
	
	float speed = distance / (float) time_interval;
	sprintf(self->board871_log_content, "Speed: %f, Distance = %f, Time interval = %f", speed, distance, (float) time_interval);
	Write_String_Log(self->board871_log_content);
	
	if (speed < MIN_SPEED) {
		self->slow++;
	} else {
		self->slow = 0;
	}
	
	if (self->slow >= SLOW_COUNTER) {
		self->slow = SLOW_COUNTER;
		self->current_node->valid = false;
		self->measure = true;
		return;
	}
	
	self->previous_node = self->current_node;
	Create_New_Node(self);
	Add_Node(self, self->previous_node);
	Print_Node(self, self->previous_node);
	sprintf(self->board871_log_content, "TOTAL NODE: %u", self->route.total_length);
	Write_String_Log(self->board871_log_content);
	self->measure = true;
}

void Add_Node(struct Board871 *self, struct Node *input_node) {
	if (!input_node || !input_node->valid) {
		return;
	}
	
	if (!self->route.node) {
		self->route.node = input_node;
	} else {
		struct Node *ptr = self->route.node;
		while (ptr->next_node) {
			ptr = ptr->next_node;
		}
		ptr->next_node = input_node;
	}
	self->route.total_length++;
}


void Get_GPS_Data(struct Board871 * self) {
	if (!self->measure) {
		return;
	}
	
	if (!Get_GPS_String(&self->lc76f)) {
		Write_String_Log("Cannot get GPS data!");
		return;
	}
	
	if (!Parse_GPS_String(&self->lc76f, self->current_node)) {
		return;
	}
}

void Get_Accel_Data(struct Board871 * self) {
	if (!self->measure) {
		return;
	}
	
	MC3416_Read_Accel(&self->mc3416, self->current_node);
	vTaskDelay(1000);
}

float DMS_To_Decimal(uint8_t degree, uint8_t minute, uint16_t second, int8_t sign) {
	float output = sign * ((float) degree + (float) minute / (60.0) + (float) second / 600000.0);
	return output;
}

float Degree_To_Rad(float degree) {
    return degree * PI / 180.0;
}

float Calculate_Distance(struct Board871 * self) {
	float lat_1 = Degree_To_Rad(DMS_To_Decimal(self->previous_node->latitude.degree, self->previous_node->latitude.minute, self->previous_node->latitude.second, self->previous_node->latitude.latitude_direction));
	float lon_1 = Degree_To_Rad(DMS_To_Decimal(self->previous_node->longitude.degree, self->previous_node->longitude.minute, self->previous_node->longitude.second, self->previous_node->longitude.longitude_direction));
	float lat_2 = Degree_To_Rad(DMS_To_Decimal(self->current_node->latitude.degree, self->current_node->latitude.minute, self->current_node->latitude.second, self->current_node->latitude.latitude_direction));
	float lon_2 = Degree_To_Rad(DMS_To_Decimal(self->current_node->longitude.degree, self->current_node->longitude.minute, self->current_node->longitude.second, self->current_node->longitude.longitude_direction));
								
	// Calculate differences between the coordinates
	float dlat = lat_2 - lat_1;
	float dlon = lon_2 - lon_1;
	
	// Apply Haversine formula
	float a, b, c;
	a = sin(dlat / 2) * sin(dlat / 2) + cos(lat_1) * cos(lat_2) * sin(dlon / 2) * sin(dlon / 2);
	c = 2 * atan2(sqrt(a), sqrt(1 - a));
	return (float) (EARTH_RADIUS * c);
}

uint32_t Calculate_Time(struct Board871 * self) {
	struct Timestamp timestamp_1 = self->previous_node->timestamp;
	struct Timestamp timestamp_2 = self->current_node->timestamp;
	
	struct tm start_time;
	start_time.tm_hour = timestamp_1.hour;    // Hour (0-23)
	start_time.tm_min = timestamp_1.minute;     // Minute (0-59)
	start_time.tm_sec = timestamp_1.second;     // Second (0-59)
	start_time.tm_mday = timestamp_1.day;    // Day of the month (1-31)
	start_time.tm_mon = timestamp_1.month;      // Month (0-11, January = 0)
	start_time.tm_year = timestamp_1.year + 2000 - 1900;   // Year since 1900

	// Define the second timestamp
	struct tm end_time;
	end_time.tm_hour = timestamp_2.hour;      // Hour (0-23)
	end_time.tm_min = timestamp_2.minute;       // Minute (0-59)
	end_time.tm_sec = timestamp_2.second;       // Second (0-59)
	end_time.tm_mday = timestamp_2.day;      // Day of the month (1-31)
	end_time.tm_mon = timestamp_2.month;        // Month (0-11, January = 0)
	end_time.tm_year = timestamp_2.year + 2000 - 1900;     // Year since 1900
	
	// Convert the timestamps to time_t values
	time_t start_t = mktime(&start_time);
	time_t end_t = mktime(&end_time);

	// Calculate the time interval in seconds
	time_t interval = end_t - start_t;

	return interval;
}

/* Debug */
void Print_Node(struct Board871 * self, struct Node *input_node) {
	if (!input_node) {
		Write_String_Log("Input node is NULL!\n");
		return;
	}
	
	char *temp;
	temp = calloc(100, sizeof(char));
	 
	sprintf(self->board871_log_content, "{");
	
	if (input_node->valid) {
		sprintf(temp, "\"valid\":true");
	} else {
		sprintf(temp, "\"valid\":false");
	}
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"timestamp\":\"%u:%u:%uT%u-%u-%u\"", input_node->timestamp.hour, input_node->timestamp.minute, input_node->timestamp.second, input_node->timestamp.day, input_node->timestamp.month, input_node->timestamp.year);
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"device_id\":\"%s\"", input_node->device_id);
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"latitude\":%f", DMS_To_Decimal(input_node->latitude.degree, input_node->latitude.minute, input_node->latitude.second, input_node->latitude.latitude_direction));
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"longitude\":%f",DMS_To_Decimal(input_node->longitude.degree, input_node->longitude.minute, input_node->longitude.second, input_node->longitude.longitude_direction));
	strcat(self->board871_log_content, temp);
	
	sprintf(temp, ",\"speed\":%u", (uint16_t) input_node->speed);
	strcat(self->board871_log_content, temp);	

	sprintf(temp, ",\"accel_x\":%d", (int16_t) input_node->accel_x);
	strcat(self->board871_log_content, temp);	

	sprintf(temp, ",\"accel_y\":%d", (int16_t) input_node->accel_y);
	strcat(self->board871_log_content, temp);	

	sprintf(temp, ",\"accel_z\":%d", (int16_t) input_node->accel_z);
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