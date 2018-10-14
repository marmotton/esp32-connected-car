#include <Arduino.h>
#include <Wire.h>
#include "functions.hpp"
#include "TempSensor.hpp"

TempSensor::TempSensor(int addr, int pin_sda, int pin_scl) {
    address = addr;
    Wire.begin(pin_sda, pin_scl);
}

float TempSensor::getTemperature() {
    return temperature;
};

void TempSensor::poll_sensor() {
    uint32_t rawTemp;
    float realTemp;

    Wire.requestFrom(address, 2);

    // data is returned as 2 bytes big endian. 0.5°C/LSB, 9 bits.
    rawTemp = 0;
    rawTemp =   Wire.read() << 1;
    rawTemp |= (Wire.read() >> 7) & 0x01;

    realTemp = twosComplementToInt(rawTemp, 9) * 0.5;

    temperature = realTemp;
};

void TempSensor::update(CarState &car) {
    poll_sensor();
    car.setIndoorTemperature(temperature);
};
