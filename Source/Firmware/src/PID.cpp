#include "PID.h"

namespace Firmware
{

PID::PID(codal::Timer &t, float Kp, float Ki, float Kd) : t{t}, Kp{Kp}, Ki{Ki}, Kd{Kd} {}

float PID::regulate(float target, float current, CODAL_TIMESTAMP current_time_ms)
{

    float delta_time_ms = current_time_ms - previous_time_ms;
    float error = target - current;
    float delta_error = error - previous_error;

    P = Kp * error;
    I += (Ki * error) * delta_time_ms;
    D = (Kd * delta_error) / delta_time_ms;

    previous_time_ms = t.getTime();

    return P + I + D;
}
} // namespace Firmware
