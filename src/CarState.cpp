#include <Arduino.h>

#include "CarState.hpp"

#include "functions.hpp"

#include "config.h"

#include <numeric>

CarState::CarState() {
    averages_semaphore = xSemaphoreCreateMutex();
};

bool CarState::gps_is_valid() {
    if ( ( millis() - gps_last_update ) < MAX_GPS_UPDATE_PERIOD_MS ) {
        return true;
    }
    else {
        return false;
    }
}

bool CarState::gsm_is_valid() {
    if ( ( millis() - gsm_last_update ) < MAX_GSM_UPDATE_PERIOD_MS ) {
        return true;
    }
    else {
        return false;
    }
}

void CarState::setBatteryStoredKWH(float kwh) {
    batteryStoredKWH = kwh;
}

void CarState::setBatteryPowerKW(float kw) {
    batteryPowerKW = kw;
};


void CarState::setGPSLatitude(float lat) {
    gps_lat = lat;
    gps_last_update = millis();
};

void CarState::setGPSLongitude(float lon) {
    gps_lon = lon;
    gps_last_update = millis();
};

void CarState::setGPSAltitude(float alt) {
    gps_alt = alt;
};

void CarState::setGPSSpeed(float spd){
    gps_spd = spd;
};


void CarState::setGSMLatitude(float lat) {
    gsm_lat = lat;
    gsm_last_update = millis();
};

void CarState::setGSMLongitude(float lon) {
    gsm_lon = lon;
    gsm_last_update = millis();
};


void CarState::setIndoorTemperature(float temp) {
    indoor_temperature = temp;
};





float CarState::getBatteryStoredKWH() {
    return batteryStoredKWH;
}

float CarState::getBatteryPowerKW() {
    return batteryPowerKW;
};

float CarState::getEconomyKWHperKM() {
    float economy = batteryPowerKW / gps_spd;

    return economy;
};


float CarState::getLatitude() {
    float lat = 9999;

    if ( gps_is_valid() ) {
        lat = gps_lat;
    }
    else if ( gsm_is_valid() ) {
        lat = gsm_lat;
    }

    return lat;
};

float CarState::getLongitude() {
    float lon = 9999;

    if ( gps_is_valid() ) {
        lon = gps_lon;
    }
    else if ( gsm_is_valid() ) {
        lon = gsm_lon;
    }

    return lon;
};

float CarState::getAltitude() {
    float alt = 9999;

    if ( gps_is_valid() ) {
        alt = gps_alt;
    }

    return alt;
};

float CarState::getSpeed() {
    float spd = 9999;

    if ( gps_is_valid() ) {
        spd = gps_spd;
    }

    return spd;
};


float CarState::getIndoorTemperature() {
    return indoor_temperature;
};

void CarState::compute() {
    if ( xSemaphoreTake( averages_semaphore, COMPUTE_EVERY_MS / portTICK_PERIOD_MS ) == pdTRUE ) {
        gps_spd_history.push_front( gps_spd );
        batteryPowerKW_history.push_front( batteryPowerKW );

        gps_spd_history.resize( COMPUTE_SAMPLES );
        batteryPowerKW_history.resize( COMPUTE_SAMPLES );

        gps_spd_avg = std::accumulate( gps_spd_history.begin(), gps_spd_history.end(), 0.0 ) / COMPUTE_SAMPLES;
        batteryPowerKW_avg = std::accumulate( batteryPowerKW_history.begin(), batteryPowerKW_history.end(), 0.0 ) / COMPUTE_SAMPLES;
        economy_avg = batteryPowerKW_avg / gps_spd_avg;

        xSemaphoreGive( averages_semaphore );
    }
};

float CarState::getBatteryPowerKWAverage() {
    return batteryPowerKW_avg;
};

float CarState::getEconomyKWHperKMAverage() {
    return economy_avg;
};

float CarState::getGPSSpeedAverage() {
    return gps_spd_avg;
};
