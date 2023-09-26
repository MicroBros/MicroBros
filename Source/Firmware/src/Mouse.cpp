#include "Mouse.h"

namespace Firmware
{

Mouse::Mouse(MicroBit &uBit, Firmware::Drivers::DFR0548 &driver) : uBit{uBit}, driver{driver} {}

// UpdateVector() assumes the compass heading is correct
void Mouse::UpdateVector()
{
    float acc_x = this->uBit.accelerometer.getX();
    float acc_y = this->uBit.accelerometer.getZ();
    float hdg_rad = NormaliseRad(Deg2Rad(this->uBit.compass.heading()));

    this->dx = acc_x * std::cos(hdg_rad) + acc_y * std::sin(hdg_rad);
    this->dy = acc_y * std::cos(hdg_rad) - acc_x * std::sin(hdg_rad);
};

void Mouse::UpdateHeading()
{
    // Shall return correct heading
    // this->rot = m_uBit.compass.heading();
}

} // namespace Firmware
