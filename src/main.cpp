#include <Arduino.h>
#include "esp_freertos_hooks.h"

// Hack to be able to use forward_list and TFT_eSPI together
#include <algorithm>
#define min(a,b) std::min(a,b)

#include "config.h"
#include "pins.h"
#include "functions.hpp"

// State of the car (similar to a global variable)
#include "CarState.hpp"
CarState car;

// CAN interface
#include "LeafCAN.hpp"
LeafCAN env200_CAN(CAN_RX, CAN_TX, CAN_SPEED_500KBPS);

// I2C temperature sensor
#include "TempSensor.hpp"
TempSensor tempIndoor(0x48, I2C_SDA, I2C_SCL);

// Display
#include "Display.hpp"
Display mainDisplay;

// GPS
#include "GPS.hpp"
GPS gps(Serial1, 9600, SERIAL_8N1, GPS_RX, GPS_TX);

// GSM and MQTT communication
#include "GsmCommunicator.hpp"
GsmCommunicator gsmComm( Serial2, GSM_RX, GSM_TX, car );


// ***********************
// Tasks
// ***********************

void canTask( void * parameter ) {
    for (;;) {
        #ifdef RUNNING_TASK_DEBUG
            Serial.println("CAN task");
        #endif
        env200_CAN.update(car); // 50ms xQueueReceive inside this method, no need for a delay()
    }
}

void gpsTask( void * parameter ) {
    for(;;) {
        #ifdef RUNNING_TASK_DEBUG
            Serial.println("GPS task");
        #endif
        gps.update(car);
        delay(50);
    }
}

void temperatureTask( void * parameter ) {
    for(;;) {
        #ifdef RUNNING_TASK_DEBUG
            Serial.println("Temperature task");
        #endif
        tempIndoor.update(car);
        delay(500);
    }
}

void displayTask( void * parameter ) {
    for(;;) {
        #ifdef RUNNING_TASK_DEBUG
            Serial.println("Display task");
        #endif
        mainDisplay.draw_display(car);
        delay(100);
    }
}

void gsmMqttTask( void * parameter ) {
    mainDisplay.setMessage( "Connecting..." );
    gsmComm.init();
    mainDisplay.setMessage( "GSM connected" );

    unsigned long lastUpdate = 0;
    float lastKWH = 0;
    unsigned long lastBalanceUpdate = 0;
    unsigned long lastGSMpositionUpdate = 0;

    for(;;) {
        #ifdef RUNNING_TASK_DEBUG
            Serial.println("GSM/MQTT task");
        #endif
        // Update in the following situations, whatever happens first:
        // Every hour
        // Battery stored kWh increased by 0.5kWh
        if (                      millis() - lastUpdate > ( 60 * 60000 )
          || car.getBatteryStoredKWH() - lastKWH > 0.5            ) {

            gsmComm.publish();

            lastUpdate = millis();
            lastKWH = car.getBatteryStoredKWH();
        }

        // Update balance once a day
        if ( millis() - lastBalanceUpdate > ( 24 * 60 * 60000) ) {
            gsmComm.updateBalance();
            gsmComm.publish();
        }

        // Update GSM location every 20s
        if ( millis() - lastGSMpositionUpdate > 20000 ) {
            gsmComm.updateLocation();
            lastGSMpositionUpdate = millis();
        }

        gsmComm.mqttLoop();

        delay(100);
    }
}

void carStateComputations( void * parameter ) {
    // Fixed execution frequency
    TickType_t lastComputeTime;


    for(;;) {
        #ifdef RUNNING_TASK_DEBUG
            Serial.println("Computation task");
        #endif
        lastComputeTime = xTaskGetTickCount();
        car.compute();
        vTaskDelayUntil( &lastComputeTime, COMPUTE_EVERY_MS / portTICK_PERIOD_MS );
    }
}

bool vApplicationIdleHook(void)
{
    #ifdef RUNNING_TASK_DEBUG
        Serial.println("Idle");
    #endif
    return false;
}


void setup() {

    // Serial port init (for debugging and SLCAN)
    Serial.begin( SERIAL_BAUDRATE );

    // SLCAN
    #ifdef SLCAN_ENABLE
        env200_CAN.enableSLCAN();
    #endif

    // Start tasks
    xTaskCreatePinnedToCore( canTask, "canTask", 2048, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore( gpsTask, "gpsTask", 2048, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore( temperatureTask, "temperatureTask", 2048, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore( displayTask, "displayTask", 2048, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore( gsmMqttTask, "gsmMqttTask", 2048, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore( carStateComputations, "carStateComputations", 2048, NULL, 4, NULL, 1);

    esp_register_freertos_idle_hook(vApplicationIdleHook);
}


void loop() {
    #ifdef RUNNING_TASK_DEBUG
        Serial.println("Arduino loop task");
    #endif
    /*
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    Serial.println(uxHighWaterMark);
    */
    delay(100);
}
