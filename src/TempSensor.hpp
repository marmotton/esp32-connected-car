#ifndef TEMPSENSOR_HPP
#define TEMPSENSOR_HPP

#include "CarState.hpp"

class TempSensor {
private:
    float temperature;
    int address;

public:
    TempSensor(int addr, int pin_sda, int pin_scl);

    float getTemperature();
    void poll_sensor();
    void update(CarState &car);
};

#endif
