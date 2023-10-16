#include "Mouse2.h"

namespace Firmware
{

Mouse2::Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver)
    : uBit{uBit}, driver{driver}, front_pid("f", 20, 0, 10), left_right_pid("lr", 20, 0, 10)
{
    std::vector<Drivers::HCSR04::Sensor> sensor_pins = {
        {.echo_pin = uBit.io.P2, .trig_pin = uBit.io.P8, .value = &fl},
        {.echo_pin = uBit.io.P0, .trig_pin = uBit.io.P1, .value = &fr}};
    ultrasonics = std::make_unique<Drivers::HCSR04>(sensor_pins);
    measurement_interval_ms = front_left->GetMeasurementInterval();
}

void Mouse2::Run()
{
    float f_avg = (fr + fl) / 2;
    while (f_avg <= 20)
    {
        active = false;
        driver->StopMotors();
        fiber_sleep(1);
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
        PerpFront();
        driver->SetMotors(bl_pwm, fl_pwm, br_pwm, fr_pwm);
    }
    active = true;
}

void Mouse2::PerpFront()
{
    float diff = fl - fr;
    float pwm = front_pid.Regulate(0, diff, measurement_interval_ms);
    fl_pwm -= Clamp<int>(0, 4095, (int)pwm);
    bl_pwm -= Clamp<int>(0, 4095, (int)pwm);
    fr_pwm += Clamp<int>(0, 4095, (int)pwm);
    br_pwm += Clamp<int>(0, 4095, (int)pwm);
}

} // namespace Firmware
