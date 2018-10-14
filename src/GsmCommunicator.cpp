#include <Arduino.h>

#include "GsmCommunicator.hpp"
#include "config.h"

GsmCommunicator::GsmCommunicator(HardwareSerial &ser, int rxpin, int txpin, CarState &carstate) : gsm_serial(ser), modem(gsm_serial), client(modem), mqtt(client), car(carstate) {
    // Setup serial port
    gsm_serial.begin(9600, SERIAL_8N1, rxpin, txpin);
}

void GsmCommunicator::mqttHandleIncoming( String topic, String payload ) {
    // Only respond to the control topic
    if ( topic == MQTT_CTRL_TOPIC ) {
        if ( payload == MQTT_BALANCE_PAYLOAD ) {
            updateBalance();
            publish();
        }
        else if ( payload == MQTT_POLL_PAYLOAD ) {
            publish();
        }
    }
}

void GsmCommunicator::updateBalance() {
    // Request balance
    String ussd_balance = modem.sendUSSD( GSM_BALANCE_USSD);

    // Extract value, including "CHF". From "CHF" to end of string
    // Full string should be e.g. "Votre crédit s'élève à CHF 4.55."
    String balance_str_tmp = strstr(ussd_balance.c_str(), "CHF");

    // Extract float value
    float balance_tmp = -999;

    if( balance_str_tmp != NULL && balance_str_tmp.length() > 4 ) {
        // Remove "CHF " and trailing ".", convert to float
        balance_tmp = atof(balance_str_tmp.substring(4, balance_str_tmp.length()-1).c_str());

        balance_chf = balance_tmp;
        balance_str = balance_str_tmp;
    }
}


bool GsmCommunicator::connectMqtt() {
    bool status = mqtt.connect( MQTT_CLIENTNAME, MQTT_USER, MQTT_PASS, "env200/status", 1, true, "offline" );

    if ( status == false ) {
        return false;
    }

    // Publish the retained "online" message ("offline" is defined as last will on mqtt.connect())
    mqtt.publish( "env200/status", "online", true );

    // Subscribe to the control topic
    mqtt.subscribe( "env200/ctrl" );

    return mqtt.connected();
}

void GsmCommunicator::init() {
    // Connect GSM and internet
    modem.init();
    modem.waitForNetwork();
    modem.gprsConnect(GPRS_APN, GPRS_USER, GPRS_PASS);

    // Connect MQTT
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);

    // mqttHandleIncoming is used as callback. The lambda function is used to call this method (and do some conversions in the meantime)
    mqtt.setCallback([this](char* topic, byte* payload, unsigned int len) {
        // Convert topic in Arduino String
        String topic_str = String( topic );

        // Convert payload in Arduino String
        char payload_chr[len+1];

        for ( int i = 0; i < len; i++ ) {
            payload_chr[i] = (char)payload[i];
        }
        // End the string with NULL
        payload_chr[len] = '\0';

        String payload_str = String( payload_chr );

        // Call the callback function
        this -> mqttHandleIncoming( topic_str, payload_str );
    });
}

void GsmCommunicator::mqttLoop() {
    if ( !mqtt.connected() ) {

        // Reconnect every 10s
        if (millis() - lastReconnectAttempt > 10000L) {
            if( connectMqtt() ) {
                lastReconnectAttempt = 0;
            }
            else {
                lastReconnectAttempt = millis();
            }
        }
        delay(100);
        return;
    }

    // Call the pubsubclient loop() method
    mqtt.loop();
}


void GsmCommunicator::publish() {
    mqtt.publish( "env200/indoorTemp", String(car.getIndoorTemperature(), 1).c_str() );

    mqtt.publish( "env200/batteryKWH", String(car.getBatteryStoredKWH(), 2).c_str() );

    mqtt.publish( "env200/lat", String(car.getLatitude(), 6).c_str() );
    mqtt.publish( "env200/lon", String(car.getLongitude(), 6).c_str() );

    mqtt.publish( "env200/balance", String(balance_chf, 2).c_str() );
}

void GsmCommunicator::updateLocation() {
    String gsmloc = modem.getGsmLocation();

    int latStartIndex = gsmloc.indexOf(',') + 1;
    int latStopIndex  = gsmloc.indexOf(',', latStartIndex) - 1;
    int lonStartIndex = gsmloc.indexOf(',', latStartIndex) + 1;
    int lonStopIndex  = gsmloc.indexOf(',', lonStartIndex) - 1;

    float lat_tmp = gsmloc.substring(latStartIndex, latStopIndex).toFloat();
    float lon_tmp = gsmloc.substring(lonStartIndex, lonStopIndex).toFloat();

    gsm_lat = lat_tmp;
    gsm_lon = lon_tmp;

    car.setGSMLatitude(gsm_lat);
    car.setGSMLongitude(gsm_lon);
}

float GsmCommunicator::getBalanceCHF() {
    return balance_chf;
}

String GsmCommunicator::getBalanceString() {
    return balance_str;
}
