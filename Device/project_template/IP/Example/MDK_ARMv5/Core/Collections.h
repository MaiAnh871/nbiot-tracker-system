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

static enum ValueType
{
	/* bool */
	VALUE_BOOL,

	/* 1 byte */
	VALUE_CHAR, VALUE_UNSIGNED_CHAR, VALUE_UINT8_T,

	/* 2 bytes */
	VALUE_SHORT, VALUE_UNSIGNED_SHORT, VALUE_UINT16_T,

	/* 4 bytes */
	VALUE_INT, VALUE_UINT32_T,

	/* 4 or 8 bytes */
	VALUE_LONG,

	/* Decimal point */
	VALUE_FLOAT,

	/* String */
	VALUE_CHAR_ARRAY

} ValueType;

// Union to store different types of values
static union Value
{
	bool *bool_value;
	char *char_value;
	unsigned char *unsigned_char_value;
	uint8_t *uint8_value;
	short *short_value;
	unsigned short *unsigned_short_value;
	uint16_t *uint16_value;
	int *int_value;
	long *long_value;
	uint32_t *uint32_value;
	float *float_value;
	char *char_array_value;
} Value;

// Struct to represent a key-value pair with a variable value datatype
typedef struct Pair
{
	char key[KEY_MAX_LENGTH];
	union Value value;
	enum ValueType value_type;
} Pair;

typedef struct DataNode
{
	struct Pair pair;
	struct DataNode *next;
} DataNode;

#endif /* COLLECTIONS_ */