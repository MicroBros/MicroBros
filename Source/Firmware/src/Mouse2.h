#pragma once
#include <MicroBit.h>

#include <Core/Mouse.h>

#include <math.h>

#include "Drivers/DFR0548.h"

#include "PID.h"

#include "Utils.h"

namespace Firmware
{

class Mouse2 : public Core::Mouse
{
public:
    Mouse2(MicroBit &uBit, Firmware::Drivers::DFR0548 *driver);
    void Run();

private:
    MicroBit &uBit;
    Firmware::Drivers::DFR0548 *driver;

    //! Distance measurements to front, left and right obstructions
    float f = 0.0;
    float l = 0.0;
    float r = 0.0;

    //! Delta distance measurements to front, left and right obstructions
    float df = 0.0;
    float dl = 0.0;
    float dr = 0.0;

    uint64_t dt_us = 0;
    float dt_s = 0.0;

    uint16_t fl_pwm = 0;
    uint16_t fr_pwm = 0;
    uint16_t bl_pwm = 0;
    uint16_t br_pwm = 0;

    uint64_t iteration;

    Firmware::PID left_right_pid;
    Firmware::PID front_pid;

    void Center();
}

} // namespace Firmware
