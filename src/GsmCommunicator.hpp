#ifndef GSMCOMMUNICATOR_HPP
#define GSMCOMMUNICATOR_HPP

#include "config.h"
#include "../lib/TinyGSM/src/TinyGsmClient.h"
#include <PubSubClient.h>

class GsmCommunicator{
private:
    HardwareSerial &gsm_serial;
    TinyGsm modem;
    TinyGsmClientSecure client;
    PubSubClient mqtt;

    bool connectMqtt();
    unsigned long lastReconnectAttempt;
    void mqttHandleIncoming(String topic, String payload);


    // GSM data
    float balance_chf;
    String balance_str;
    float gsm_lat;
    float gsm_lon;

    // Temperature sensor
    float indoor_temperature;

    // Car data
    float battery_kwh;
    bool doors_are_locked;

    // GPS data
    time_t last_fix;
    float gps_lat;
    float gps_lon;
    float gps_alt;
    float hdop;

public:
    GsmCommunicator(HardwareSerial &ser, int rxpin, int txpin);

    void init();

    void mqttLoop();

    float getBalanceCHF();
    String getBalanceString();
    void updateBalance();

    float getLiPoVoltage();
    float getLiPoPercent();

    void setIndoorTemperature(float temp);

    void setBatteryKWH(float kwh);
    void setDoorsAreLocked(bool doorsAreLocked);

    void setLatitude(float lat);
    void setLongitude(float lon);
    void setAltitude(float alt);
    void setHDOP(float hdop);
    void setLastFix(time_t lastFix);

    void publish();
};

#endif
