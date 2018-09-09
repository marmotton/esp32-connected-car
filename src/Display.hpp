#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <TFT_eSPI.h>


class Display{
private:
    int speed;
    float batteryPercent;
    float batteryKWH;
    float batteryKW;
    float temperature;
    String message;

    bool speedIsValid;

    TFT_eSPI tft;
    TFT_eSprite img;

public:
    Display();
    void draw_display();
    void setSpeed(float spd);
    void setBatteryPercent(float batt);
    void setBatteryKWH(float kwh);
    void setBatteryKW(float kw);
    void setTemperature(float temp);
    void setMessage(String msg);

    void setSpeedIsValid(bool state);
};

#endif
