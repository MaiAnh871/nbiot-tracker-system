#ifndef SETTINGS_
#define SETTINGS_

/* Testing */
// Uncomment the following line for testing
#define TEST_ENABLE

#ifdef TEST_ENABLE
	#define TEST_GPS_STRING false
	#define TEST_NODE_DATA false
	#define TEST_ACCEL false
	#define TEST_CURRENT_CONSUMPTION true /* In stage 5, MCU will sleep and need to reset MCU */
#else
	#define TEST_GPS_STRING false
	#define TEST_NODE_DATA false
	#define TEST_ACCEL false
	#define TEST_CURRENT_CONSUMPTION false
#endif

/* Math */
#define PI 3.141592653589793238
#define EARTH_RADIUS 6378160.0 // m

/* System */
#define CURRENT_TICK xTaskGetTickCount()

/* Debugging */
static
const char * SUCCESS_COMMAND_SIGN[] = {
  "\r\n\r\n",
  "OK\r\n"
};
static
const char * ERROR_COMMAND_SIGN[] = {
  "ERROR"
};
#define SUCCESS_COMMAND_SIGN_LENGTH sizeof(SUCCESS_COMMAND_SIGN) / sizeof(SUCCESS_COMMAND_SIGN[0])
#define ERROR_COMMAND_SIGN_LENGTH sizeof(ERROR_COMMAND_SIGN) / sizeof(ERROR_COMMAND_SIGN[0])

/* Utilities settings */
#define MAXIMUM_TOKEN_COUNT 15

/* BOARD871 settings */
#define BAUD_RATE_LOG 115200
#define BOARD871_LOG_CONTENT 2000
#define KEY_MAX_LENGTH 30
#define DEVICE_ID_LENGTH 18
#define LATITUDE_LENGTH 9 		/* 'ddmm.mmmm' */
#define LONGTITUDE_LENGTH 10	/* 'dddmm.mmmm' */
#define CELL_ID_LENGTH 9 /* '00000000' - 'FFFFFFFF' */

#define VALIDATE_PERIOD 7000 // ms
#define MIN_SPEED 0.1 // m/s
#define SPEED_LIMIT 60 // km/h
#define SLOW_COUNTER 3
#define DATA_NODE_STRING_LENGTH 500
#define DOUBLE_CHECK_NETWORK_PERIOD 10000 // ms
#define DOUBLE_CHECK_SPEED_PERIOD 60000 // ms
#define TIME_ZONE 7
#define TILT_THRESHOLD 45 // degree (ax)
#define WHEELIE_THRESHOLD 30 // degree (ay)
#define MAX_TOTAL_NODE 60

/* BC660K settings */
#define BC660K_LOG_CONTENT_SIZE 2000
#define BC660K_SEND_ATTEMPT_DEFAULT 3
#define BC660K_COMMAND_TIMEOUT_DEFAULT_MS 2000
#define BC660K_COMMAND_SIZE 2000
#define BC660K_RECEIVE_BUFFER_SIZE 500
#define BC660K_SEND_COMMAND_DELAY_MS 500

/* LC76F settings */
#define LC76F_LOG_CONTENT_SIZE 300
#define LC76F_GPS_DATA_SIZE 100
#define LC76F_TEMP_STRING_LENGTH 10

/* MC3416 settings */
#define MC3416_LOG_CONTENT_SIZE 300

#define MC3416_ADDR 0x4C
#define MC3416_HTCFG_I2C_MASTER_CLK(CKCUClock) CKCUClock.Bit.I2C0
#define MC3416_HTCFG_I2C_MASTER_SCL_GPIO_ID GPIO_PB
#define MC3416_HTCFG_I2C_MASTER_SCL_AFIO_PIN AFIO_PIN_1
#define MC3416_HTCFG_I2C_MASTER_SDA_GPIO_ID GPIO_PB
#define MC3416_HTCFG_I2C_MASTER_SDA_AFIO_PIN AFIO_PIN_0
#define MC3416_I2C_MASTER_ADDRESS 0
#define MC3416_CLOCK_SPEED 100000
#define MC3416_HTCFG_I2C_MASTER_PORT HT_I2C0

#define MC3416_OFFSET_AX 24
#define MC3416_OFFSET_AY 254
#define MC3416_OFFSET_AZ 25

#define MC3416_MOVEMENT_THRESHOLD 1500

#endif /* SETTINGS_ */