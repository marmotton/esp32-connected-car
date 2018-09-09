#include <Arduino.h>

#include "GPS.hpp"
#include "functions.hpp"

GPS::GPS( HardwareSerial &serial_dev, int baudrate, int serial_config, int rx_pin, int tx_pin, int expectedUpdateMS ) : serial_device(serial_dev) {
    serial_device.begin(baudrate, serial_config, rx_pin, tx_pin);

    gpsTimer = xTimerCreate( "gpsTimer", expectedUpdateMS / portTICK_PERIOD_MS, pdFALSE, NULL, timerCallbackThatDoesNothing );
    xTimerStart(gpsTimer, 10);
}

void GPS::update() {
    // Interpret the data coming from the GPS module
    while ( serial_device.available() ) {
        gps.encode( serial_device.read() );
    }

    // Update the speed
    if ( gps.speed.isUpdated() ) {
        float speed_tmp = gps.speed.kmph();
        speed = speed_tmp;
    }

    // Update the altitude
    if ( gps.altitude.isUpdated() ) {
        float altitude_tmp = gps.altitude.meters();
        altitude = altitude_tmp;
    }

    // Update HDOP
    if ( gps.hdop.isUpdated() ) {
        float hdop_tmp = gps.hdop.hdop();
        hdop = hdop_tmp;
    }

    // Update the location and reset the GPS timeout counter
    if ( gps.location.isUpdated() ) {
        float longitude_tmp = gps.location.lng();
        float latitude_tmp  = gps.location.lat();

        longitude = longitude_tmp;
        latitude = latitude_tmp;

        xTimerReset( gpsTimer, 10 );
    }
}

bool GPS::isValid() {
    if ( xTimerIsTimerActive( gpsTimer ) == pdTRUE ) {
        return true;
    }
    else {
        return false;
    }
}

float GPS::getLatitude() {
    return latitude;
}

float GPS::getLongitude() {
    return longitude;
}

float GPS::getAltitude() {
    return altitude;
}

float GPS::getSpeed() {
    return speed;
}

float GPS::getHDOP() {
    return hdop;
}
