#pragma once

#include <functional>
#include <memory>
#include <span>

#include <MicroBit.h>

#include "../Timer.h"

#define IR_SAMPLE_SIZE 512
#define IR_SAMPLE_TYPE float

namespace Firmware::Drivers
{

/*! \brief IR diode based distance
 *
 * \attention The \p value inside of IR::Sensor MUST be allocated on heap as it is updated from
 * a different fiber, otherwise issues may arrise
 */

class IR
{
public:
    struct Sensor
    {
        //! Pin for IR sensor diode
        NRF52Pin &sense_pin;
        //! Value to update with the distance in cm
        float *value;
    };

    /*! \brief Constructor for IR
        - \p sensors is a vector of pins for *sense* and a pointer to value to update
       with a float distance in (cm) as argument
        - \p measurement_rate Rate in Hz to do measurements
     */
    IR(std::vector<Sensor> sensors, NRF52Pin &emitter_pin, uint16_t measurement_rate = 60);

    //! Returns true if the last measurement was done within the interval
    bool IsMeasuring();
    inline uint16_t GetSamplingRate() { return sample_rate; }
    //! Run a cycle of signal processing with filters
    void RunSignalProcessing();

private:
    //! Run a measurement
    void Run();

    std::vector<Sensor> sensors;
    std::vector<std::array<IR_SAMPLE_TYPE, IR_SAMPLE_SIZE>> raw_samples;
    std::unique_ptr<Timer> timer;
    CODAL_TIMESTAMP last_measurement;
    uint16_t sample_rate;
    size_t prev_idx{0};
    size_t idx{0};
};

}; // namespace Firmware::Drivers
