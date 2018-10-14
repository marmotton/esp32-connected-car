#include <Arduino.h>

#include <TFT_eSPI.h>

#include "Display.hpp" // Adafruit_ST7735.h is already included in display.hpp

Display::Display() : tft(), img(&tft) {
    tft.init();
    tft.setRotation(3);
    tft.fillScreen( TFT_BLACK );
}

void Display::draw_display(CarState &car) {

    img.setColorDepth(8);
    img.createSprite( TFT_HEIGHT, TFT_WIDTH );
    img.fillSprite( TFT_BLACK );

    img.setTextColor( TFT_WHITE );

    // *******************
    // Speed display
    // *******************
    int speed = (int)car.getSpeed();
    img.setFreeFont( &FreeSansBold24pt7b );
    img.setTextDatum( MR_DATUM );
    // If speed is valid
    if ( car.gps_is_valid() && speed < 500 ) {
        img.drawNumber(speed, 85, 16);
    }
    else {
        img.drawString("---", 85, 16);
    }

    img.setFreeFont(&FreeSans9pt7b);
    img.setTextDatum( ML_DATUM );
    img.drawString( "km / h", 95, 16 );

    // *******************
    // Economy display (kWh/100km)
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );

    float power = car.getBatteryPowerKW();

    if ( power > 0 ) {
        // Charging
        img.setTextColor( TFT_GREEN );
    }

    if ( car.gps_is_valid() && speed > 0 && speed < 500 ) {
        float economy = car.getEconomyKWHperKMAverage() * 100;
        img.drawFloat(economy, 1, 44, 58);
    }
    else {
        img.drawString("---", 44, 58);
    }

    img.setTextFont(1);
    img.setTextDatum( ML_DATUM );
    img.drawString("kWh", 53, 55);
    img.drawString("100km", 47, 65);
    img.drawLine(49, 59, 74, 59, TFT_WHITE);

    img.setTextColor( TFT_WHITE );


    // *******************
    // Battery kWh display
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );
    img.drawFloat(car.getBatteryStoredKWH(), 1, 130, 58);

    img.setFreeFont( &FreeSans9pt7b );
    img.setTextFont(1);
    img.setTextDatum( ML_DATUM );
    img.drawString("kWh", 133, 65);


    // *******************
    // Temperature display
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );
    img.drawNumber( car.getIndoorTemperature(), 40, 90 );

    img.setFreeFont( &FreeSans9pt7b );
    img.setTextDatum( ML_DATUM );
    img.drawString( "C", 51, 90 );
    img.drawCircle(46, 86, 3, TFT_WHITE); // degree symbol


    // *******************
    // Battery kW display
    // *******************
    img.setFreeFont( &FreeSansBold12pt7b );
    img.setTextDatum( MR_DATUM );
    if ( power > 0 ) {
        // Charging
        img.setTextColor( TFT_GREEN );
        img.drawFloat( power, 1, 130, 90 );
    }
    else {
        // Discharging
        img.drawFloat( -power, 1, 130, 90 );
    }

    img.setFreeFont( &FreeSans9pt7b );
    img.setTextDatum( ML_DATUM );
    img.drawString( "kW", 133, 90 );
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


void Display::setMessage(String msg) {
    message = msg;
}
