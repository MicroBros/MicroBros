#include <Core/Log.h>

#include "PID.h"

namespace Firmware
{

PID::PID(codal::Timer &t, std::string name, float Kp, float Ki, float Kd)
    : t{t}, name{name}, Kp{Kp}, Ki{Ki}, Kd{Kd}
{
    previous_time_ms = t.getTime();
}

float PID::Regulate(float target, float current, CODAL_TIMESTAMP current_time_ms)
{

    float delta_time_ms = current_time_ms - previous_time_ms;
    float error = target - current;
    float delta_error = error - previous_error;

    P = Kp * error;
    I += (Ki * error) * delta_time_ms;

    if (delta_time_ms > 0)
    {
        D = (Kd * delta_error) / delta_time_ms;
    }
    previous_time_ms = t.getTime();

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
