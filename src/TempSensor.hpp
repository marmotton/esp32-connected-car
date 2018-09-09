#ifndef TEMPSENSOR_HPP
#define TEMPSENSOR_HPP

class TempSensor {
private:
    float temperature;
    int address;

public:
    TempSensor(int addr, int pin_sda, int pin_scl);

    float getTemperature();
    void poll_sensor();
};

#endif
