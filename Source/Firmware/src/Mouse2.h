#pragma once
#include <MicroBit.h>

#include <Core/Mouse.h>

#include <math.h>

#include "Drivers/DFR0548.h"
#include "Drivers/HCSR04.h"
#include "PID.h"
#include "Utils.h"

namespace Firmware
{

class Mouse2 : public Core::Mouse
{
public:
    Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver);
    void Run();

private:
    MicroBit &uBit;
    Drivers::DFR0548 *driver;
    std::unique_ptr<Drivers::HCSR04> ultrasonics;

    bool active = false;

    //! Distance measurements to front, left and right obstructions
    float f{0.0f};
    float l{0.0f};
    float r{0.0f};

    uint16_t measurement_interval_ms;

    uint16_t fl_pwm = 2048;
    uint16_t fr_pwm = 2048;
    uint16_t bl_pwm = 2048;
    uint16_t br_pwm = 2048;

    PID left_right_pid;
    PID front_pid;

    void PerpFront();
    void CenterSides();
};

} // namespace Firmware
