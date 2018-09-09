#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "functions.hpp"

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
GPS gps(Serial1, 9600, SERIAL_8N1, GPS_RX, GPS_TX, MAX_GPS_UPDATE_PERIOD_MS);

// GSM and MQTT communication
#include "GsmCommunicator.hpp"
GsmCommunicator gsmComm( Serial2, GSM_RX, GSM_TX );


// ***********************
// Tasks
// ***********************

void canTask( void * parameter ) {
    for (;;) {
        env200_CAN.update(); // 50ms xQueueReceive inside this method, no need for a delay()
    }
}

void gpsTask( void * parameter ) {
    for(;;) {
        //Serial.println("gpsTask");
        gps.update();
        delay(50);
    }
}

void temperatureTask( void * parameter ) {
    for(;;) {
        //Serial.println("temperatureTask");
        tempIndoor.poll_sensor();
        delay(500);
    }
}

void displayTask( void * parameter ) {
    for(;;) {
        //Serial.println("displayTask");
        mainDisplay.setTemperature( tempIndoor.getTemperature() );

        if ( gps.isValid() ) {
            mainDisplay.setSpeed( gps.getSpeed() );
            mainDisplay.setSpeedIsValid( true );
        }
        else {
            mainDisplay.setSpeed( 0 );
            mainDisplay.setSpeedIsValid( false );
        }

        mainDisplay.setBatteryPercent( env200_CAN.getBatteryPercent() );
        mainDisplay.setBatteryKWH( env200_CAN.getBatteryStoredKWH() );
        mainDisplay.setBatteryKW( env200_CAN.getBatteryPower() );
/*
        mainDisplay.setBatteryKWH( 20.2 );
        mainDisplay.setBatteryPercent( 100 );
        mainDisplay.setBatteryKW( 1.772 );
        mainDisplay.setSpeed( 128 );
        mainDisplay.setSpeedIsValid( true );
        mainDisplay.setTemperature( -28.5 );
*/
        mainDisplay.draw_display();

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

    for(;;) {
        // Update in the following situations, whatever happens first:
        // Every hour
        // Battery stored kWh increased by 0.5kWh
        if (                      millis() - lastUpdate > ( 60 * 60000 )
          || env200_CAN.getBatteryStoredKWH() - lastKWH > 0.5            ) {

            gsmComm.setLongitude( gps.getLongitude() );
            gsmComm.setLatitude( gps.getLatitude() );

            gsmComm.setBatteryKWH( env200_CAN.getBatteryStoredKWH() );

            gsmComm.setIndoorTemperature( tempIndoor.getTemperature() );

            gsmComm.publish();

            lastUpdate = millis();
            lastKWH = env200_CAN.getBatteryStoredKWH();
        }

        // Update balance once a day
        if ( millis() - lastBalanceUpdate > ( 24 * 60 * 60000) ) {
            gsmComm.updateBalance();
            gsmComm.publish();
        }

        gsmComm.mqttLoop();
        delay(100);
    }
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
    xTaskCreatePinnedToCore( gsmMqttTask, "gsmMqttTask", 2048, NULL, 5, NULL, 1);
}


void loop() {
    delay(100);

    // if ( digitalRead(0) == 0 )
    // {
    //     //ESP.deepSleep(5e6);
    //     //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_2M);
    //     //esp_sleep_enable_timer_wakeup(5e6);
    //     //esp_light_sleep_start();
    //     //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
    // }
}
