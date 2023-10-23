#include <Core/Log.h>

#include "Mouse2.h"

namespace Firmware
{

Mouse2::Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver)
    : uBit{uBit}, driver{driver}, front_pid("f", 5, 0, 0), left_right_pid("lr", 25, 0, 0)
{
    std::vector<Drivers::HCSR04::Sensor> sensor_pins = {
        //{.echo_pin = uBit.io.P0, .trig_pin = uBit.io.P2, .value = &f},
        {.echo_pin = uBit.io.P8, .trig_pin = uBit.io.P1, .value = &r},
        {.echo_pin = uBit.io.P13, .trig_pin = uBit.io.P14, .value = &l}};

    ultrasonics = std::make_unique<Drivers::HCSR04>(sensor_pins);
    measurement_interval_ms = ultrasonics->GetMeasurementInterval();
}

void Mouse2::Run()
{
    LOG_INFO("f: {}, l: {}, r: {}", f, l, r);
    LOG_INFO("measure={}", measurement_interval_ms);

    if (l <= 4 || r <= 4)
    {
        active = false;
        driver->StopMotors();
        return;
    }

    if (!active)
    {
        bl_pwm = 2048;
        fl_pwm = 2048;
        br_pwm = 2048;
        fr_pwm = 2048;
    }
    else
    {
        // PerpFront();
        CenterSides();
        driver->SetMotors(bl_pwm, fl_pwm, br_pwm, fr_pwm);
    }
    active = true;
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

void Mouse2::CenterSides()
{
    float diff = l - r;
    int pwm = left_right_pid.Regulate(0, diff, measurement_interval_ms);
    fl_pwm += pwm;
    bl_pwm -= pwm;
    fr_pwm -= pwm;
    br_pwm += pwm;
    LOG_INFO("Center_PWM: fl={}, bl={}, fr={}, br={}", fl_pwm, bl_pwm, fr_pwm, br_pwm);
    left_right_pid.Debug();
}

} // namespace Firmware
