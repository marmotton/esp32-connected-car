#ifndef GSMCOMMUNICATOR_HPP
#define GSMCOMMUNICATOR_HPP

#include "config.h"
#include "../lib/TinyGSM/src/TinyGsmClient.h"
#include <PubSubClient.h>

#include "CarState.hpp"

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

    // State of the car (position, temperature, battery...)
    CarState &car;

public:
    GsmCommunicator(HardwareSerial &ser, int rxpin, int txpin, CarState &carstate);

    void init();

    void mqttLoop();

    float getBalanceCHF();
    String getBalanceString();
    void updateBalance();

    float getLiPoVoltage();
    float getLiPoPercent();

    void updateLocation();

    void publish();
};

#endif
