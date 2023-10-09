#pragma once
#include <MicroBit.h>

#include <Core/Mouse.h>

#include <math.h>

#include "Drivers/DFR0548.h"

#include "PID.h"

#include "Utils.h"

namespace Firmware
{

class Mouse : public Core::Mouse
{

public:
    Mouse(MicroBit &uBit, Firmware::Drivers::DFR0548 &driver);

    void Debug();
    void Run();

private:
    MicroBit &uBit;
    Firmware::Drivers::DFR0548 &driver;

    //! Distance sensors states
    float distance_fl = 0;
    float distance_fr = 0;
    float distance_l = 0;
    float distance_r = 0;

    //! PID objects dedicated to each sensor
    Firmware::PID fl_pid;
    Firmware::PID fr_pid;
    Firmware::PID l_pid;
    Firmware::PID r_pid;

    //! Acceleration, velocity, position relative to Earth (MAYBE NOT NEEDED)
    float acceleration_x = 0;
    float acceleration_y = 0;
    float velocity_x = 0;
    float velocity_y = 0;
    float position_x = 0;
    float position_y = 0;

    //! Delta time of the current state in microseconds
    uint64_t delta_time_us = 0;
    float delta_time_s = 0;

    uint64_t iteration = 0;

    //! Zero = North, Positive direction = Clockwise
    void UpdateHeading();
    void UpdateAcceleration();
    void UpdateVelocity();
    void UpdatePosition();
    void Perp();
};

} // namespace Firmware
