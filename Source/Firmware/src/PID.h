#include <Timer.h>

namespace Firmware
{

class PID
{

public:
    //! Constructor takes input arguments PID constants
    PID(codal::Timer &t, float Kp, float Ki, float Kd);

    float regulate(float target, float current, CODAL_TIMESTAMP current_time_ms);

private:
    CODAL_TIMESTAMP previous_time_ms;
    codal::Timer &t;
    float previous_error;
    float P = 0;
    float I = 0;
    float D = 0;

    float Kp;
    float Ki;
    float Kd;
};
} // namespace Firmware
