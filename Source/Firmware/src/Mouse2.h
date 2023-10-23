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

    //! Get if the mouse should be running
    inline bool IsRunning() noexcept { return running; }
    //! Set if the mouse is running
    inline void SetRunning(bool running) noexcept { this->running = running; }

private:
    MicroBit &uBit;
    Drivers::DFR0548 *driver;
    std::unique_ptr<Drivers::HCSR04> ultrasonics;
    float MAZE_SIZE = 16.0f;

    //! True if the Mouse2 is running autonomously, set false for manual control
    bool running{false};

    //! Distance measurements to front, left and right obstructions
    float f{0.0f};
    float l{0.0f};
    float r{0.0f};

    int iter = 0;

    uint16_t measurement_interval_ms;

    float forward_pwm = 0.0f;
    float right_pwm = 0.0f;
    float rot_pwm = 0.0f;
    uint16_t fl_pwm;
    uint16_t fr_pwm;
    uint16_t bl_pwm;
    uint16_t br_pwm;

    PID rot_pid;
    PID right_pid;
    PID forward_pid;

    void PerpFront();
    void CenterSides();
    void Forward();
    void SetPWM();
};

} // namespace Firmware
