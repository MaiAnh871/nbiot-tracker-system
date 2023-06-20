#ifndef COLLECTIONS_
#define COLLECTIONS_

#include "stdbool.h"
#include "stdint.h"
#include "Setting.h"

static enum StatusType {
  STATUS_SUCCESS = 0,
    STATUS_ERROR,
    STATUS_TIMEOUT,
    STATUS_BAD_PARAMETERS,
    STATUS_UNKNOWN
}
StatusType;

static char * getStatusTypeString(enum StatusType status) {
  switch (status) {
  case STATUS_SUCCESS:
    return "SUCCESS";
  case STATUS_ERROR:
    return "ERROR";
  case STATUS_TIMEOUT:
    return "TIMEOUT";
  case STATUS_BAD_PARAMETERS:
    return "PARAMETERS";
  case STATUS_UNKNOWN:
    return "UNKNOWN";
  default:
    return "UNSUPPORTED STATUS";
  }
}

static struct Timestamp {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} Timestamp;

static struct Connection_Status {
	int16_t rsrp;
	char cell_id[CELL_ID_LENGTH];
	int8_t sinr;
} Connection_Status;

static struct Node {
	struct Timestamp timestamp;
	char device_id[DEVICE_ID_LENGTH];
	char latitude[LATITUDE_LENGTH];
	char longtitude[LONGTITUDE_LENGTH];
	uint16_t speed; /* distance_last_node / time_interval_last_node */
	double accel_x;
	double accel_y;
	double accel_z;
	bool tilt_alert;
	bool overspeed_alert;
	uint16_t speed_limit;
	struct Connection_Status connection_status;
} Node;

static struct Route {
	double total_length;
	struct Node *node;
} Route;

#endif /* COLLECTIONS_ */