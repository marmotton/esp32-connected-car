#ifndef CONFIG_H
#define CONFIG_H

// Enable functions (comment out to disable)
#define SLCAN_ENABLE // SLCAN output on serial port

// Car parameters
#define KWH_PER_GID 0.08
#define BATT_MAX_KWH 21.3
#define COMPUTE_ECONOMY_ABOVE_KMH 1 // must be >0

// GPS parameters
#define MAX_GPS_UPDATE_PERIOD_MS 1500 // Consider gps data not valid if no location update during this period

// USB serial output
//#define SERIAL_BAUDRATE 115200
//#define SERIAL_BAUDRATE 230400
#define SERIAL_BAUDRATE 460800
//#define SERIAL_BAUDRATE 921600

// GSM
#define GPRS_APN "internet"
#define GPRS_USER ""
#define GPRS_PASS ""
#define GSM_BALANCE_USSD "*147#"

// GSM modem
#define TINY_GSM_MODEM_SIM800

// MQTT
#define MQTT_SERVER "myserver.org"
#define MQTT_PORT 8883
#define MQTT_USER "myUser"
#define MQTT_PASS "myPassword"
#define MQTT_CLIENTNAME "esp32car"

#define MQTT_CTRL_TOPIC "smartcar/control"
#define MQTT_BALANCE_PAYLOAD "bal"
#define MQTT_POLL_PAYLOAD "update"

#endif
