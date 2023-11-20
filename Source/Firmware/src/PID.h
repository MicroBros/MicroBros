#include <Timer.h>

namespace Firmware
{

/*! \brief PID controller
 */
class PID
{

public:
    //! Constructor takes input arguments PID constants
    PID(std::string name, float Kp, float Ki, float Kd);

    float Regulate(float target, float current, uint16_t measurement_interval_ms);

    void Debug();

private:
    uint16_t measurement_interval_ms;
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
