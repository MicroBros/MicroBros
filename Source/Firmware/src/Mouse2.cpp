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

    ultrasonics = std::make_unique<Drivers::HCSR04>(sensor_pins);
    measurement_interval_ms = ultrasonics->GetMeasurementInterval();

    // TODO: Uncomment when tested, this will initialise FloodFill as default algorithm
    // SetAlgorithm("FloodFill");
}

void Mouse2::Run()
{
    // if (iter % 500000 == 0)
    //{

    LOG("Iter:{}", iter);
    LOG("Distances: f={}\t l={}\t, r={}\n", f, l, r);
    LOG("DT={}\n", measurement_interval_ms);
    Forward();
    PerpFront();
    CenterSides();
    SetPWM();
    //}
    ++iter;
    driver->SetMotors(bl_pwm, fl_pwm, br_pwm, fr_pwm);
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
        SetAlgorithm(algorithm);
}

/*
void Mouse2::PerpFront()
{
    float diff = fl - fr;
    int pwm = front_pid.Regulate(0, diff, measurement_interval_ms);
    fl_pwm -= pwm;
    bl_pwm -= pwm;
    fr_pwm += pwm;
    br_pwm += pwm;
    LOG_INFO("Perp_PWM: fl={}, bl={}, fr={}, br={}", fl_pwm, bl_pwm, fr_pwm, br_pwm);
}
*/

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
    float target = 4.0;
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

    float diff = l - r;
    rot_pwm = rot_pid.Regulate(0, diff, measurement_interval_ms);
    rot_pid.Debug();
}

} // namespace Firmware
