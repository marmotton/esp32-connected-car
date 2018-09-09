#include <Arduino.h>

#include "LeafCAN.hpp"
#include "ESP32CAN.h" // https://github.com/nhatuan84/arduino-esp32-can-demo

#include "CAN_config.h"

#include "functions.hpp"

CAN_device_t CAN_cfg;

LeafCAN::LeafCAN(gpio_num_t rx_pin, gpio_num_t tx_pin, CAN_speed_t speed) {
    CAN_cfg.speed = speed;
    CAN_cfg.tx_pin_id = tx_pin;
    CAN_cfg.rx_pin_id = rx_pin;
    CAN_cfg.rx_queue = xQueueCreate( 10, sizeof( CAN_frame_t ) );

    slcan_enabled = false;

    ESP32Can.CANInit();
}

void LeafCAN::update() {
    CAN_frame_t canRxFrame;

    if ( xQueueReceive( CAN_cfg.rx_queue, &canRxFrame, 50 / portTICK_PERIOD_MS ) ) {

        // Output the received CAN frame to the serial port (SLCAN format)
        if ( slcan_enabled ) {
            printf( "t%03x%d", canRxFrame.MsgID, canRxFrame.FIR.B.DLC );
            for ( int i = 0; i < canRxFrame.FIR.B.DLC; i++ ){
                printf( "%02x", canRxFrame.data.u8[i] );
            }
            printf("\r");
        }


        // Update battery charge level (message 0x5BC)
        if ( canRxFrame.MsgID == 0x5BC ) {
            // Extract the 10 first bits of the message (byte[0] + 2 first bits of byte[1])
            int gids_tmp = ( canRxFrame.data.u8[0] << 2 ) | ( canRxFrame.data.u8[1] >> 6 );
            gids = gids_tmp;
        }

        // Update battery power
        if ( canRxFrame.MsgID == 0x1DB ) {
            float current = 0.5 * twosComplementToInt( ( ( canRxFrame.data.u8[0] << 3 ) | ( canRxFrame.data.u8[1] >> 5 ) ), 11 ); // 11 MSBs, 0.5A per LSB, 2's complement
            float voltage = 0.5 * ( ( canRxFrame.data.u8[2] << 2 ) | ( canRxFrame.data.u8[3] >> 6 ) ); // 10 bits, 0.5V per LSB

            float power_tmp = 0.001 * current * voltage;
            batteryPower = power_tmp;
        }

        // TODO: Update doors locked state

        // TODO: Update charge cable state

        // TODO: Update climate control state

        //
    }
}

int LeafCAN::getGIDS() {
    return gids;
}

float LeafCAN::getBatteryStoredKWH() {
    float kwh_tmp = ( (float) gids ) * kwh_per_gid;
    return kwh_tmp;
}

float LeafCAN::getBatteryPercent() {
    float percent_tmp = getBatteryStoredKWH() / batt_max_kwh * 100;
    return percent_tmp;
}

float LeafCAN::getBatteryPower() {
    return batteryPower;
}

void LeafCAN::enableSLCAN() {
    slcan_enabled = true;
}

void LeafCAN::disableSLCAN() {
    slcan_enabled = false;
}
