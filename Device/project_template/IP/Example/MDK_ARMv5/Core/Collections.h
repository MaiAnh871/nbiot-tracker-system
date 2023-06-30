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

static struct Time {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} Time;

static struct Connection_Status {
	int16_t rsrp;
	char cell_id[CELL_ID_LENGTH];
	int8_t sinr;
} Connection_Status;

static enum Longitude_Direction {
	WEST = -1,
	EAST = 1
} Longitude_Direction;

static struct Longitude {
	uint8_t degree; 	/* 0 - 180 (standardized) */
	uint8_t minute; 	/* 0 - 60 */
	int second;		/* 0 - 9999 ~ 0 - 60 */
	enum Longitude_Direction longitude_direction;
} Longitude;

static enum Latitude_Direction {
	SOUTH = -1,
	NORTH = 1
} Latitude_Direction;

static struct Latitude {
	uint8_t degree; 	/* 0 - 90 (standardized) */
	uint8_t minute; 	/* 0 - 60 */
	int second;		/* 0 - 9999 ~ 0 - 60 */
	enum Latitude_Direction latitude_direction;
} Latitude;

static struct Node {
	bool valid;
	struct Timestamp timestamp;
	char device_id[DEVICE_ID_LENGTH];
	struct Latitude latitude;
	struct Longitude longitude;
	float speed; /* distance_last_node / time_interval_last_node */
	float accel_x;
	float accel_y;
	float accel_z;
	bool tilt_alert;
	bool overspeed_alert;
	uint16_t speed_limit;
	struct Connection_Status connection_status;
	struct Time send_data_interval;
	struct Time alive_interval;
	struct Node *next_node;
} Node;

static struct Route {
	float total_length;
	struct Node *node;
} Route;

static struct Accel {
	int16_t Ax;
	int16_t Ay;
	int16_t Az;
} Accel;

#endif /* COLLECTIONS_ */