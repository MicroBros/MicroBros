#include <Timer.h>

namespace Firmware
{

class PID
{

public:
    //! Constructor takes input arguments PID constants
    PID(codal::Timer &t, std::string name, float Kp, float Ki, float Kd);

    float Regulate(float target, float current, CODAL_TIMESTAMP current_time_ms);

    void Debug();

private:
    CODAL_TIMESTAMP previous_time_ms;
    codal::Timer &t;
    std::string name;
    float previous_error = 0;
    float P = 0;
    float I = 0;
    float D = 0;

    float Kp;
    float Ki;
    float Kd;
};
} // namespace Firmware
