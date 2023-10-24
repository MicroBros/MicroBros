#pragma once
#include <MicroBit.h>

#include <Core/Mouse.h>

#include <math.h>

#include <deque>

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
    //! External class objects
    MicroBit &uBit;
    Drivers::DFR0548 *driver;
    std::unique_ptr<Drivers::HCSR04> ultrasonics;
    int sensor_count;
    uint64_t prev_time_ms;
    //! Filtering-related variables
    std::deque<float> distance_queue; // Saving latest distance readings for filtering reasons
    static const int FILTER_SIZE = 3; // Number of readings for moving average
    static const float THRESHOLD = 0.05; // Change in distance threshold
    static const int DEBOUNCE_COUNT = 3; // Number of continous increases before stopping
    float prevAverageDistance = 0;
    int increasingCount = 0;
    const float MAZE_SIZE = 16.0f;

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

    void TurnRight();
    bool FindMinima();

    float MovingAverageFilter(float distance);
};

} // namespace Firmware
