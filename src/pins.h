#ifndef PINS_H
#define PINS_H

// TFT display
#define TFT_CS   14
#define TFT_RST  -1
#define TFT_DC   27
#define TFT_SCLK 13
#define TFT_MOSI 12

// GSM SIM800L
#define GSM_TX 33
#define GSM_RX 34

// GPS BN-180
#define GPS_TX  0 // Not connected. Is -1 possible here ?
#define GPS_RX 36

// Temperature sensor
#define I2C_SCL 22
#define I2C_SDA 21

// CANBUS
#define CAN_TX GPIO_NUM_25
#define CAN_RX GPIO_NUM_39

// SD-card
#define SD_DI  23
#define SD_DO  19
#define SD_CLK 18
#define SD_nCS  4

#endif
