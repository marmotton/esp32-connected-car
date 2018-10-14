#ifndef LEAFCAN_HPP
#define LEAFCAN_HPP

#include "config.h"

#include "CAN_config.h"

#include "CarState.hpp"

class LeafCAN {
private:
    const float kwh_per_gid = KWH_PER_GID;
    const float batt_max_kwh = BATT_MAX_KWH;

    int gids;
    float batteryPower;

    bool slcan_enabled;

public:
    LeafCAN(gpio_num_t rx_pin, gpio_num_t tx_pin, CAN_speed_t speed);
    int getGIDS();
    float getBatteryStoredKWH();
    float getBatteryPercent();
    float getBatteryPower();

    bool doorsAreLocked();
    bool climateControlIsOn();
    bool isCharging();

    void startClimateControl();
    void stopClimateControl();

    void update(CarState &car);

    void enableSLCAN();
    void disableSLCAN();
};

#endif
