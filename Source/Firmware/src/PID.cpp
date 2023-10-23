#include <Core/Log.h>

#include "PID.h"

namespace Firmware
{

PID::PID(std::string name, float Kp, float Ki, float Kd) : name{name}, Kp{Kp}, Ki{Ki}, Kd{Kd} {}

float PID::Regulate(float target, float current, uint16_t measurement_interval_ms)
{

    float error = target - current;
    float delta_error = error - previous_error;

    P = Kp * error;
    I += (Ki * error) * measurement_interval_ms;
    D = (Kd * delta_error) / measurement_interval_ms;

    return P + I + D;
}

void Firmware::PID::Debug()
{
    LOG("Name: {}\t", name);
    LOG("DT: {}\t", measurement_interval_ms);
    LOG("P: {}\t", P);
    LOG("I: {}\t", I);
    LOG("D: {}\t", D);
    LOG("P+I+D: {}\n", P + I + D);
    LOG("--------------------------------\n");
}

} // namespace Firmware
