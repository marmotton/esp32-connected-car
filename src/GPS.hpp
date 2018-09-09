#ifndef GPS_HPP
#define GPS_HPP

#include <time.h>
#include <HardwareSerial.h>

#include <TinyGPS++.h>

class GPS {
private:
    TinyGPSPlus gps;

    TimerHandle_t gpsTimer;

    time_t lastFix;
    time_t currentTime;

    float latitude;
    float longitude;
    float altitude;
    float speed;
    float hdop;

    HardwareSerial &serial_device;

public:
    GPS( HardwareSerial &serial_dev, int baudrate, int serial_config, int rx_pin, int tx_pin, int expectedUpdateMS );

    void update();

    bool isValid();

    time_t getLastFixTime();
    time_t getCurrentTime();

    float getLatitude();
    float getLongitude();
    float getAltitude();
    float getSpeed();
    float getHDOP();
};

#endif
