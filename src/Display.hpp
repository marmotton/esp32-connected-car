#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <TFT_eSPI.h>

#include "CarState.hpp"


class Display{
private:
    String message;

    TFT_eSPI tft;
    TFT_eSprite img;

public:
    Display();
    void draw_display(CarState &car);
    void setMessage(String msg);
};

#endif
