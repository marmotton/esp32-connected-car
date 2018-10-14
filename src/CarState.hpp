#ifndef CARSTATE_HPP
#define CARSTATE_HPP

#include <time.h>
#include <forward_list>


class CarState {
private:
    float batteryStoredKWH;
    float batteryPowerKW;

    float gps_lat;
    float gps_lon;
    float gps_alt;
    float gps_spd;
    unsigned long gps_last_update;

    float gsm_lat;
    float gsm_lon;
    unsigned long gsm_last_update;

    float indoor_temperature;

    // Averages
    SemaphoreHandle_t averages_semaphore;
    std::forward_list<float> gps_spd_history;
    std::forward_list<float> batteryPowerKW_history;
    float gps_spd_avg;
    float batteryPowerKW_avg;
    float economy_avg;

public:
    CarState();
    void setBatteryStoredKWH(float kwh);
    void setBatteryPowerKW(float kw);

    void setGPSLatitude(float lat);
    void setGPSLongitude(float lon);
    void setGPSAltitude(float alt);
    void setGPSSpeed(float spd);

    void setGSMLatitude(float lat);
    void setGSMLongitude(float lon);

    void setIndoorTemperature(float temp);



    float getBatteryStoredKWH();
    float getBatteryPowerKW();
    float getBatteryPowerKWAverage();
    float getEconomyKWHperKM();
    float getEconomyKWHperKMAverage();

    float getLatitude();
    float getLongitude();
    float getAltitude();
    float getSpeed();
    float getGPSSpeedAverage();

    float getIndoorTemperature();

    bool gps_is_valid();
    bool gsm_is_valid();

    void compute();
};

#endif
