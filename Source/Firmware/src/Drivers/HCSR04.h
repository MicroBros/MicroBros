#pragma once

#include <functional>
#include <memory>
#include <span>

#include <MicroBit.h>

#include "../Filters.h"
#include "../Timer.h"

namespace Firmware::Drivers
{

/*! \brief HC-SR04 ultrasonic ranging module driver
 *
 * The module is capable of 2-400cm of measurement
 *
 * - HC-SR04 datasheet: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
 *
 * \attention The \p value inside of HCSR04::Sensor MUST be allocated on heap as it is updated from
 * a different fiber, otherwise issues may arrise
 */

class HCSR04
{
public:
    struct Sensor
    {
        //! echo_pin is the PIN where the common *Echo *signals are sent
        NRF52Pin &echo_pin;
        //! Pin for the trigger for sensor module
        NRF52Pin &trig_pin;
        //! Value to update with the distance in cm
        float *value;

        Filters::MovingAverageFilter<float, 3> filter{};
    };

    /*! \brief Constructor for HCSR04
        - \p sensors is a vector of pins for *Trig* (Trigger) and a pointer to value to update
       with a float distance in (cm) as argument
        - \p measurement_interval How often in milliseconds a sensor should be triggered
     */
    HCSR04(std::vector<Sensor> sensors, uint16_t measurement_interval = 60);

    //! Returns true if the last measurement was done within the interval
    bool IsMeasuring();
    inline uint16_t GetMeasurementInterval() { return measurement_interval; }

private:
    //! Run a measurement
    void Run();
    //! Event handler for high pulses
    void OnPulse(Event event);

    std::vector<Sensor> sensors;
    std::vector<CODAL_TIMESTAMP> last_measurements;
    std::unique_ptr<Timer> timer;
    uint16_t measurement_interval;
    uint16_t idx{0};
};

}; // namespace Firmware::Drivers
