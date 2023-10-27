#include <Core/Log.h>

#include "Mouse2.h"

namespace Firmware
{

Mouse2::Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver)
    : uBit{uBit}, driver{driver}, forward_pid("f", -25, 0, 0), right_pid("lr", 50, 0, 0),
      rot_pid("rot", 0, 0, 250)
{
    std::vector<Drivers::HCSR04::Sensor> sensor_pins = {
        {.echo_pin = uBit.io.P0, .trig_pin = uBit.io.P2, .value = &f},
        {.echo_pin = uBit.io.P8, .trig_pin = uBit.io.P1, .value = &r},
        {.echo_pin = uBit.io.P13, .trig_pin = uBit.io.P14, .value = &l}};

    sensor_count = sensor_pins.size();
    prev_time_ms = uBit.timer.getTime();
    ultrasonics = std::make_unique<Drivers::HCSR04>(sensor_pins);
    measurement_interval_ms = ultrasonics->GetMeasurementInterval();

    // Initialise FloodFill as default algorithm
    SetAlgorithm("FloodFill");
}

void Mouse2::Run()
{

    // Debug
    LOG("Iter:{}", iter);
    LOG("Distances: f={}\t l={}\t, r={}\n", f, l, r);
    LOG("DT={}\n", measurement_interval_ms);

    // Sense

    // Think
    Forward();
    PerpFront();
    CenterSides();

    // Act
    SetPWM();
    driver->SetMotors(bl_pwm, fl_pwm, br_pwm, fr_pwm);

    //
    ++iter;
}

void Mouse2::Step()
{
    // TODO: This will start a step, basically the time from a sensor reading until the mouse is
    // going to do the next one updating the map
    // In short, it is a one tile move
}

void Mouse2::Reset()
{
    Mouse::Reset();

    // Update algorithm if set manually
    if (algorithm >= 0)
    {
        if (!SetAlgorithm(algorithm))
            LOG_ERROR("Error setting algorithm: {}", algorithm);
    }
}

void Mouse2::SetPWM()
{
    float denominator{
        std::max(std::abs(forward_pwm) + std::abs(right_pwm) + std::abs(rot_pwm), 1.0f)};
    fr_pwm = static_cast<int16_t>((forward_pwm - right_pwm - rot_pwm) / denominator * 4095.0f);
    br_pwm = static_cast<int16_t>((forward_pwm + right_pwm - rot_pwm) / denominator * 4095.0f);
    fl_pwm = static_cast<int16_t>((forward_pwm + right_pwm + rot_pwm) / denominator * 4095.0f);
    bl_pwm = static_cast<int16_t>((forward_pwm - right_pwm + rot_pwm) / denominator * 4095.0f);
    LOG("SetPWM: fr={}\tbr={}\tfl={}\tbl={}\n", fr_pwm, br_pwm, fl_pwm, bl_pwm);
}

void Mouse2::Forward()
{
    float target = 10.0;
    forward_pwm = forward_pid.Regulate(target, f, measurement_interval_ms);
    forward_pid.Debug();
}

// Measures diff between left and right readings, sets right_pwm
void Mouse2::CenterSides()
{
    float diff = l - r;
    right_pwm = right_pid.Regulate(0, diff, measurement_interval_ms);
    right_pid.Debug();
}

void Mouse2::PerpFront()
{
    float diff = std::fmod(l, MAZE_SIZE) - std::fmod(r, MAZE_SIZE);
    rot_pwm = rot_pid.Regulate(0, diff, measurement_interval_ms);
    rot_pid.Debug();
}

void Mouse2::TurnRight()
{
    fr_pwm = -2048;
    br_pwm = -2048;
    fl_pwm = 2048;
    bl_pwm = 2048;
}

bool Mouse2::FindMinima()
{
    prevAverageDistance = f;

    while (true)
    {
        float avg_distance = MovingAverageFilter(f);
        if (avg_distance > prevAverageDistance + THRESHOLD)
        {
            ++increasingCount;
            if (increasingCount + DEBOUNCE_COUNT)
            {
                break;
            }
        }
        else
        {
            increasingCount = 0;
        }
        prevAverageDistance = 0;
    }
}

float Mouse2::MovingAverageFilter(float distance)
{
    // Time check to ensure a new distance value is registered before proceeding
    if (uBit.timer.getTime() - prev_time_ms >= measurement_interval_ms * sensor_count)
    {
        if (distance_queue.size() == FILTER_SIZE)
        {
            distance_queue.pop_front();
        }
        distance_queue.push_back(distance);

        float sum = 0.0;
        for (float d : distance_queue)
        {
            sum += d;
        }
        return sum / distance_queue.size();
    }
    prev_time_ms = uBit.timer.getTime();
}

} // namespace Firmware
