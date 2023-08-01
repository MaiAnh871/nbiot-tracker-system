#include "Board871.h"

void Board871_Initialize(struct Board871 * self) {
	self->stage = 0;
  self -> board871_log_content = (char * ) malloc(BOARD871_LOG_CONTENT * sizeof(char));
  if (!self -> board871_log_content) {
		Error_Blinking_LED_1();
  }
	
  self -> data_string = (char * ) malloc(DATA_NODE_STRING_LENGTH * sizeof(char));
  if (!self -> data_string) {
		Error_Blinking_LED_1();
  }
	
  self -> temp = (char * ) malloc(100 * sizeof(char));
  if (!self -> temp) {
		Error_Blinking_LED_1();
  }

  BC660K_Initialize(&self->bc660k);
  LC76F_Initialize(&self->lc76f);
  MC3416_Initialize(&self->mc3416);
	
	self->slow = 0;
	self->route.total_length = 0;
	
	Create_New_Node(self);
	self->previous_node = self->current_node;
	
	strcpy(self->connection_status.cell_id, "00000000");
	self->connection_status.rsrp = 0;
	
	self->has_gps = false;
}

void Suspend_Measuring(struct Board871 *self) {
	vTaskSuspend(TaskHandle_1);
	vTaskSuspend(TaskHandle_2);
}

void Resume_Measuring(struct Board871 *self) {
	vTaskResume(TaskHandle_1);
	vTaskResume(TaskHandle_2);
}

void Create_New_Node(struct Board871 * self) {
	self->current_node = malloc (sizeof(Node));
	
	self->current_node->valid = false;
	
	self->current_node->coordinates.timestamp.hour = 0;
	self->current_node->coordinates.timestamp.minute = 0;
	self->current_node->coordinates.timestamp.second = 0;
	self->current_node->coordinates.timestamp.day = 1;
	self->current_node->coordinates.timestamp.month = 1;
	self->current_node->coordinates.timestamp.year = 1970;
	
	sprintf(self->current_node->device_id, "%s", "861536030196001");
	
	self->current_node->coordinates.latitude.degree = 0;
	self->current_node->coordinates.latitude.minute = 0;
	self->current_node->coordinates.latitude.second = 0;
	self->current_node->coordinates.latitude.latitude_direction = NORTH;
	
	self->current_node->coordinates.longitude.degree = 0;
	self->current_node->coordinates.longitude.minute = 0;
	self->current_node->coordinates.longitude.second = 0;
	self->current_node->coordinates.longitude.longitude_direction = EAST;
	
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
	
	Pack_Node_Data(self, input_node);
	strcpy(self->board871_log_content, self->data_string);
	Write_String_Log(self->board871_log_content);
}

void Validate_Node(struct Board871 *self) {
	Suspend_Measuring(self);
	
	if (!self->current_node) {
		Create_New_Node(self);
		Resume_Measuring(self);
		Write_String_Log("Current Node is Null!");
		return;
	}
	
	if (!self->current_node->valid) {
		Resume_Measuring(self);
		return;
	}
	
	if (!self->route.node) {
		Add_Node(self, self->previous_node);
		Create_New_Node(self);
		Resume_Measuring(self);
		return;
	}
	
	if (self->current_node->speed < MIN_SPEED) {
		self->slow++;
	} else {
		self->slow = 0;
	}
	
	if ((self->current_node->speed * 3.6) > SPEED_LIMIT) {
		self->current_node->overspeed_alert = true;
	}
	
	if (self->slow >= SLOW_COUNTER) {
		Write_String_Log("self->slow >= SLOW_COUNTER");
		self->slow = SLOW_COUNTER;
		self->current_node->valid = false;
		Resume_Measuring(self);
		return;
	}
	
	self->previous_node = self->current_node;
	Create_New_Node(self);
//	Print_Node(self, self->previous_node);
	Add_Node(self, self->previous_node);
	
	if (TEST_NODE_DATA) {
		Print_Node(self, self->previous_node);
		sprintf(self->board871_log_content, "TOTAL NODE: %u", self->route.total_length);
		Write_String_Log(self->board871_log_content);
	}
	
	Resume_Measuring(self);
}

void Add_Node(struct Board871 *self, struct Node *input_node) {
	if (!input_node || !input_node->valid) {
		return;
	}
	
	sprintf(self->board871_log_content, "ROUTE LENGTH: %u", self->route.total_length);
	Write_String_Log(self->board871_log_content);
	
	if (!self->route.node) {
		self->route.node = input_node;
	} else {
		if (self->route.total_length >= MAX_TOTAL_NODE) {
			sprintf(self->board871_log_content, "NUMBER OF NODE EXCEEDS LIMITATION (%u): %u. Remove the head of Route!", MAX_TOTAL_NODE, self->route.total_length);
			Write_String_Log(self->board871_log_content);
			struct Node *temp = self->route.node;
			if (self->publishing_node == self->route.node) {
				self->publishing_node = self->route.node->next_node;
			}
			self->route.node = self->route.node->next_node;
			self->route.total_length--;
			free(temp);
		}
		struct Node *ptr = self->route.node;
		while (ptr->next_node) {
			ptr = ptr->next_node;
		}
		ptr->next_node = input_node;
	}
	self->route.total_length++;
}

void Clear_String_Data(struct Board871 * self) {
	int i;
	for (i = 0; i < DATA_NODE_STRING_LENGTH; i++) {
		self->data_string[i] = 0;
	}
}

void Get_GPS_Data(struct Board871 * self) {
	if (!Get_GPS_String(&self->lc76f)) {
//		Write_String_Log("Cannot get GPS data!");
		return;
	}
	
	self->has_gps = true;
	Parse_GPS_String(&self->lc76f, self->current_node);
}

void Get_Accel_Data(struct Board871 * self) {
	MC3416_Read_Accel(&self->mc3416, self->current_node);
	vTaskDelay(1000);
}

void Pack_Node_Data(struct Board871 * self, struct Node *input_node) {
	if (!input_node) {
		Write_String_Log("Input node is NULL!\n");
		return;
	}

//	sprintf(self->board871_log_content, "Packing data...");
//	Write_String_Log(self->board871_log_content);
	
//	sprintf(self->board871_log_content, "1");
//	Write_String_Log(self->board871_log_content);
	 
	Clear_String_Data(self);
	
	sprintf(self->data_string, "{\"message\":{");
	
//	sprintf(self->board871_log_content, "2");
//	Write_String_Log(self->board871_log_content);
	
	if (input_node->valid) {
		sprintf(self->temp, "\"valid\":true");
	} else {
		sprintf(self->temp, "\"valid\":false");
	}
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "3");
//	Write_String_Log(self->board871_log_content);
	
	sprintf(self->temp, ",\"timestamp\":\"%u:%u:%uT%u-%u-%u\"", input_node->coordinates.timestamp.hour, input_node->coordinates.timestamp.minute, input_node->coordinates.timestamp.second, input_node->coordinates.timestamp.day, input_node->coordinates.timestamp.month, input_node->coordinates.timestamp.year);
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "4");
//	Write_String_Log(self->board871_log_content);
	
	sprintf(self->temp, ",\"device_id\":\"%s\"", input_node->device_id);
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "5");
//	Write_String_Log(self->board871_log_content);
	
	sprintf(self->temp, ",\"latitude\":%f", DMS_To_Decimal(input_node->coordinates.latitude.degree, input_node->coordinates.latitude.minute, input_node->coordinates.latitude.second, input_node->coordinates.latitude.latitude_direction));
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "6");
//	Write_String_Log(self->board871_log_content);
	
	sprintf(self->temp, ",\"longitude\":%f",DMS_To_Decimal(input_node->coordinates.longitude.degree, input_node->coordinates.longitude.minute, input_node->coordinates.longitude.second, input_node->coordinates.longitude.longitude_direction));
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "7");
//	Write_String_Log(self->board871_log_content);
	
	sprintf(self->temp, ",\"speed\":%f", input_node->speed);
	strcat(self->data_string, self->temp);	
	
//	sprintf(self->board871_log_content, "8");
//	Write_String_Log(self->board871_log_content);

	sprintf(self->temp, ",\"accel_x\":%d", (int16_t) input_node->accel_x);
	strcat(self->data_string, self->temp);

//	sprintf(self->board871_log_content, "9");
//	Write_String_Log(self->board871_log_content);

	sprintf(self->temp, ",\"accel_y\":%d", (int16_t) input_node->accel_y);
	strcat(self->data_string, self->temp);	
	
//	sprintf(self->board871_log_content, "10");
//	Write_String_Log(self->board871_log_content);

	sprintf(self->temp, ",\"accel_z\":%d", (int16_t) input_node->accel_z);
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "11");
//	Write_String_Log(self->board871_log_content);
	
	if (input_node->tilt_alert) {
		sprintf(self->temp, ",\"tilt_alert\":true");
	} else {
		sprintf(self->temp, ",\"tilt_alert\":false");
	}
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "12");
//	Write_String_Log(self->board871_log_content);
	
	if (input_node->wheelie_alert) {
		sprintf(self->temp, ",\"wheelie_alert\":true");
	} else {
		sprintf(self->temp, ",\"wheelie_alert\":false");
	}
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "13");
//	Write_String_Log(self->board871_log_content);
	
	if (input_node->overspeed_alert) {
		sprintf(self->temp, ",\"overspeed_alert\":true");
	} else {
		sprintf(self->temp, ",\"overspeed_alert\":false");
	}
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "14");
//	Write_String_Log(self->board871_log_content);	
	
	sprintf(self->temp, ",\"connection_status\":{\"cell_id\":\"%s\",\"rsrp\":%d}", input_node->connection_status.cell_id, input_node->connection_status.rsrp);
	strcat(self->data_string, self->temp);
	
//	sprintf(self->board871_log_content, "15");
//	Write_String_Log(self->board871_log_content);
	
	strcat(self->data_string, "}}");
	
//	sprintf(self->board871_log_content, "Packed data!");
//	Write_String_Log(self->board871_log_content);

//	Write_String_Log(self->data_string);
}

void Connection_Flow(struct Board871 *self) {
	int8_t attempt;
	int8_t count;
	/* Initial stage */
	while (self->stage == 0) {
		Write_String_Log("\n========= STAGE 0 ========= \n");		
		if (checkModule_AT(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		for (count = 0; count < 4; count++) {
			checkModule_AT(&self->bc660k);
		}
		
		if (offEcho_ATE0(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (configureSleepMode_AT_QSCLK(&self->bc660k, 0) != STATUS_SUCCESS) {
			continue;
		}		
		
		if (powerSavingModeSetting_AT_CPSMS(&self->bc660k, 0)) {
			continue;
		}
		
		if (setAuthentication_AT_QSSLCFG(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (setCACert_AT_QSSLCFG(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (setClientCert_AT_QSSLCFG(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (setClientPrivateKey_AT_QSSLCFG(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (enableSSL_AT_QMTCFG(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (configureEDRX_AT_QEDRXCFG(&self->bc660k, 1) != STATUS_SUCCESS) {
			continue;
		}
		
		if (checkEDRX_AT_CEDRXRDP(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		self->stage = 1;
	}
	
	/* Connecting stage */
	while (self->stage == 1) {
		Write_String_Log("\n========= STAGE 1 ========= \n");
		if (configureSleepMode_AT_QSCLK(&self->bc660k, 0) != STATUS_SUCCESS) {
			continue;
		}
		
		attempt = 10;
		count = attempt;
		while (count--) {
			sprintf(self->board871_log_content, "Checking GPS: %u/%u", attempt - count, attempt);
			Write_String_Log(self->board871_log_content);
			
			if (self->has_gps) {
				break;
			} else {
				vTaskDelay(7000);
			}
		}
		
		if (count == -1) {
			self->stage = 4;
			break;
		}

		attempt = 8;
		count = attempt;
		while (count--) {
			sprintf(self->board871_log_content, "Attempt: %u/%u", attempt - count, attempt);
			Write_String_Log(self->board871_log_content);
			if (checkNetworkRegister_AT_CEREG(&self->bc660k) != STATUS_SUCCESS) {
				continue;
			}
			
			if (self->bc660k.stat == 1) {
				break;
			} else {
				vTaskDelay(3000);
			}
		}
		
		if (count == -1) {
			self->stage = 3;
			break;
		}
		
		getNetworkStatus_AT_QENG(&self->bc660k);
		
		attempt = 3;
		count = attempt;
		
		while (count--) {
			sprintf(self->board871_log_content, "Attempt: %u/%u", attempt - count, attempt);
			Write_String_Log(self->board871_log_content);
			if (checkMQTT_AT_QMTOPEN(&self->bc660k) != STATUS_SUCCESS) {
				continue;
			}
			
			if (self->bc660k.mqtt_opened) {
				break;
			} else {
				openMQTT_AT_QMTOPEN(&self->bc660k);
				
				if (self->bc660k.mqtt_opened) {
					connectClient_AT_QMTCONN(&self->bc660k);
					self->stage = 2;
					break;
				}
				
			}
		}
		
		if (self->stage == 2) {
			break;
		}
		
		if (count == -1) {
			closeMQTT_AT_QMTCLOSE(&self->bc660k);
			continue;
		}
		
		if (checkConnectClient_AT_QMTCONN(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (self->bc660k.mqtt_connected) {
			self->stage = 2;
			break;
		} else {
			connectClient_AT_QMTCONN(&self->bc660k);
		}
		
		self->stage = 2;
	}
	
	/* Publishing stage */
	while (self->stage == 2) {
		Write_String_Log("\n========= STAGE 2 ========= \n");
		
		if (!self->publishing_node) {
			Write_String_Log("Waiting for validated head node!");
			self->publishing_node = self->route.node;
			vTaskDelay(1000);
			continue;
		}
		
		if (!self->publishing_node->valid) {
			Write_String_Log("Waiting for a validated node!");
			vTaskDelay(1000);
			continue;
		}

		if (getNetworkStatus_AT_QENG(&self->bc660k) == STATUS_SUCCESS) {
//			sprintf(self->board871_log_content, "SUCCESS");
//			Write_String_Log(self->board871_log_content);
//			sprintf(self->board871_log_content, "CELL ID: %s  |  RSRP: %d", self->bc660k.connection_status.cell_id, self->bc660k.connection_status.rsrp);
//			Write_String_Log(self->board871_log_content);
			strcpy(self->connection_status.cell_id, self->bc660k.connection_status.cell_id);
			self->connection_status.rsrp = self->bc660k.connection_status.rsrp;
		} else {
//			sprintf(self->board871_log_content, "FAIL");
//			Write_String_Log(self->board871_log_content);
		}
		
		sprintf(self->board871_log_content, "BC660K - CELL_ID: %s, RSRP: %d", self->bc660k.connection_status.cell_id, self->bc660k.connection_status.rsrp);
		Write_String_Log(self->board871_log_content);
		
		strcpy(self->publishing_node->connection_status.cell_id, self->connection_status.cell_id);
		self->publishing_node->connection_status.rsrp = self->connection_status.rsrp;
		
		Pack_Node_Data(self, self->publishing_node);
		
		attempt = 5;
		count = attempt;
		
		while (count--) {
			sprintf(self->board871_log_content, "Attempt: %u/%u", attempt - count, attempt);
			Write_String_Log(self->board871_log_content);
			if (publishMessage_AT_QMTPUB(&self->bc660k, self->data_string) != STATUS_SUCCESS) {
				continue;
			}
		}
		
		if (count == -1) {
			self->stage = 1;
			break;
		}
		
		Write_String_Log("Published!");
		
		if (self->publishing_node->next_node) {
			struct Node *temp_node = self->publishing_node;
			Write_String_Log("Assigned temp_node");
			self->route.node = self->publishing_node->next_node;
			Write_String_Log("Assigned temp_node");
			self->publishing_node = self->publishing_node->next_node;
			Write_String_Log("Changed to next node");
			free(temp_node);
			Write_String_Log("Freed previous node");
			self->route.total_length--;
		} else {
			if (!(MC3416_Moving(&self->mc3416) && self->has_gps)) {
				Write_String_Log("\nDETECT NO MOVEMENT OR NO GPS!");
				closeMQTT_AT_QMTCLOSE(&self->bc660k);
				self->stage = 4;
			}
			
			vTaskDelay(1000);
		}
	}
	
	/* No network, double check movement */
	uint32_t movement_timer = CURRENT_TICK;
	uint32_t network_timer = CURRENT_TICK;
	bool entry_step_3 = true;
	while (self->stage == 3) {
		if (entry_step_3) {
			Write_String_Log("\n========= STAGE 3 ========= \n");
			entry_step_3 = false;
		}
		
		if (MC3416_Moving(&self->mc3416)) {
			movement_timer = CURRENT_TICK;
			
			if (CURRENT_TICK - network_timer >= DOUBLE_CHECK_NETWORK_PERIOD) {
				network_timer = CURRENT_TICK;
				sprintf(self->board871_log_content, "Double checking network each %d ms while detecting movement...", DOUBLE_CHECK_NETWORK_PERIOD);
				Write_String_Log(self->board871_log_content);
				if (checkNetworkRegister_AT_CEREG(&self->bc660k) != STATUS_SUCCESS) {
					continue;
				}
				
				if (self->bc660k.stat == 1) {
					self->stage = 1;
				}
				
			}
		} else {
			if (CURRENT_TICK - movement_timer >= DOUBLE_CHECK_SPEED_PERIOD) {
				sprintf(self->board871_log_content, "No movement in %d ms!", DOUBLE_CHECK_SPEED_PERIOD);
				Write_String_Log(self->board871_log_content);
				self->stage = 4;
			}
		}
		
		vTaskDelay(100);
	}
	
	/* No network, no movement => Power saving mode */
	bool entry_step_4 = true;
	while (self->stage == 4) {
		if (entry_step_4) {
			Write_String_Log("\n========= STAGE 4 ========= \n");
			entry_step_4 = false;
		}
		
		/* Sleep module */
		if (powerSavingModeSetting_AT_CPSMS(&self->bc660k, 1) != STATUS_SUCCESS) {
			continue;
		}
		
		if (enableNBIoTRelatedEventReport(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (controlReportDeepSleep(&self->bc660k) != STATUS_SUCCESS) {
			continue;
		}
		
		if (configureSleepMode_AT_QSCLK(&self->bc660k, 1) != STATUS_SUCCESS) {
			continue;
		}
				
		/* Suspend other tasks */
		LC76F_Standby(&self->lc76f);
		Suspend_Measuring(self);
		vTaskSuspend(TaskHandle_3);
		if (TEST_CURRENT_CONSUMPTION) {
			PWRCU_DeepSleep1(PWRCU_SLEEP_ENTRY_WFI);
		}
		
		/* Log would not be written if MCU is sleeping */
		Write_String_Log("\nSLEEPING.....\n");
		
		while (!MC3416_Moving(&self->mc3416)) {
			/* Interrupt hardware does not work. Better use external interrupt! */
			if (MC3416_Moving(&self->mc3416)) {
				/* Wake module up */
				if (checkModule_AT(&self->bc660k) != STATUS_SUCCESS) {
					continue;
				}
				
				for (count = 0; count < 4; count++) {
					checkModule_AT(&self->bc660k);
				}
				
				if (powerSavingModeSetting_AT_CPSMS(&self->bc660k, 0)) {
					continue;
				}
				
				if (configureSleepMode_AT_QSCLK(&self->bc660k, 0) != STATUS_SUCCESS) {
					continue;
				}		
				
				/* Resume other tasks */
				Write_String_Log("\nWAKING UP.....\n");
				LC76F_Wakeup(&self->lc76f);
				Resume_Measuring(self);
				vTaskResume(TaskHandle_3);	
			
				self->stage = 3;
				break;
			}
		}
		
		vTaskDelay(100);
	}
}
