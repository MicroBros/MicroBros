#pragma once

#include <functional>
#include <memory>
#include <span>

#include <MicroBit.h>

#include "../Filters.h"
#include "../Timer.h"
#include "../Utils.h"

#define IR_SAMPLE_SIZE 512
#define IR_SAMPLE_TYPE float
#define IR_SINK_SIZE 2

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
        //! Base value (Distance when wood is obstructing on side, next to wheels)
        float base;
        //! Scale (to make it more cm like)
        float scale;
        //! Exp to apply before scale
        float exp;
    };

    /*! \brief Constructor for IR
        - \p sensors is a vector of pins for *sense* and a pointer to value to update
       with a float distance in (cm) as argument
        - \p sample_rate Rate in Hz to do samples
     */
    IR(std::vector<Sensor> sensors, NRF52Pin &emitter_pin, uint16_t sample_rate = 4800);

    inline uint16_t GetSamplingRate() { return sample_rate; }
    //! Run a cycle of signal processing with filters
    void RunSignalProcessing();

private:
    struct SensorData
    {
        std::unique_ptr<NRF52ADCChannel> adc_channel;
        std::unique_ptr<Filters::BandpassFilter> bandpass;
        std::unique_ptr<Filters::AbsoluteFilter> abs;
        std::unique_ptr<CircularSink<IR_SINK_SIZE>> sink;
        std::unique_ptr<codal::LowPassFilter> lowpass;
    };

    //! Run a measurement
    void Run();

    std::vector<Sensor> sensors;
    std::vector<SensorData> data;
    uint16_t sample_rate;
    size_t prev_idx{0};
    size_t idx{0};
};

}; // namespace Firmware::Drivers
