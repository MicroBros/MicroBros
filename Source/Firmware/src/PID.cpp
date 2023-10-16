#include <Core/Log.h>

#include "PID.h"

namespace Firmware
{

PID::PID(std::string name, float Kp, float Ki, float Kd) : name{name}, Kp{Kp}, Ki{Ki}, Kd{Kd} {}

float PID::Regulate(float target, float current, uint16_t measurement_interval_ms)
{

    float error = target - current;
    float delta_error = error - previous_error;
    float dt_s = measurement_interval_ms / 1000;

    P = Kp * error;
    I += (Ki * error) * dt_s;
    D = (Kd * delta_error) / dt_s;

    return P + I + D;
}

void Firmware::PID::Debug()
{
    LOG("Name: {}\t", name);
    LOG("P: {}\t", P);
    LOG("I: {}\t", I);
    LOG("D: {}\t", D);
    LOG("P+I+D: {}\n", P + I + D);
    LOG("--------------------------------\n");
}

} // namespace Firmware
