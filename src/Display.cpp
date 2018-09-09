#include <Arduino.h>

#include <TFT_eSPI.h>

#include "Display.hpp" // Adafruit_ST7735.h is already included in display.hpp

Display::Display() : tft(), img(&tft) {
    tft.init();
    tft.setRotation(3);
    tft.fillScreen( TFT_BLACK );
}

void Display::draw_display() {

    img.setColorDepth(8);
    img.createSprite( TFT_HEIGHT, TFT_WIDTH );
    img.fillSprite( TFT_BLACK );

    img.setTextColor( TFT_WHITE );

    // *******************
    // Speed display
    // *******************
    img.setFreeFont( &FreeSansBold24pt7b );
    img.setTextDatum( MR_DATUM );
    if ( speedIsValid ) {
        img.drawNumber(speed, 85, 16);
    }
    else {
        img.drawString("---", 85, 16);
    }

    img.setFreeFont(&FreeSans9pt7b);
    img.setTextDatum( ML_DATUM );
    img.drawString( "km / h", 95, 16 );


    // *******************
    // Battery % display
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );
    img.drawNumber(batteryPercent, 40, 58);

    img.setFreeFont( &FreeSans9pt7b );
    img.setTextDatum( ML_DATUM );
    img.drawString("\%", 43, 58);


    // *******************
    // Battery kWh display
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );
    img.drawFloat(batteryKWH, 1, 121, 58);

    img.setFreeFont( &FreeSans9pt7b );
    img.setTextDatum( ML_DATUM );
    img.drawString("kWh", 124, 58);


    // *******************
    // Temperature display
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );
    img.drawNumber( temperature, 40, 90 );

    img.setFreeFont( &FreeSans9pt7b );
    img.setTextDatum( ML_DATUM );
    img.drawString( "C", 51, 90 );
    img.drawCircle(46, 86, 3, TFT_WHITE); // degree symbol


    // *******************
    // Battery kW display
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );
    if ( batteryKW > 0 ) {
        // Charging
        img.setTextColor( TFT_GREEN );
        img.drawFloat( batteryKW, 1, 121, 90 );
    }
    else {
        // Discharging
        img.drawFloat( -batteryKW, 1, 121, 90 );
    }

    img.setFreeFont( &FreeSans9pt7b );
    img.setTextDatum( ML_DATUM );
    img.drawString( "kW", 124, 90 );
    img.setTextColor( TFT_WHITE );


    // *******************
    // Message display
    // *******************
    img.fillRect( 0, 108, 160, 20, TFT_ORANGE );

    img.setTextColor( TFT_BLACK );
    img.setFreeFont( &FreeSerif9pt7b );
    img.setTextDatum( ML_DATUM );
    img.drawString( message, 1, 118 );

    img.pushSprite(0, 0);

    img.deleteSprite();

}


void Display::setSpeed(float spd) {
    speed = spd;
}

void Display::setBatteryPercent(float batt) {
    batteryPercent = batt;
}

void Display::setBatteryKWH(float kwh) {
    batteryKWH = kwh;
}

void Display::setBatteryKW(float kw) {
    batteryKW = kw;
}

void Display::setTemperature(float temp) {
    temperature = temp;
}

void Display::setMessage(String msg) {
    message = msg;
}

void Display::setSpeedIsValid(bool state) {
    speedIsValid = state;
}
