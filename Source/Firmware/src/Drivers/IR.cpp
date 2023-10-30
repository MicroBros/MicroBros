#include <algorithm>

#include "../Filters.h"
#include "IR.h"

extern MicroBit uBit;

const size_t SAMPLES_PER_FLASH{2};

namespace Firmware::Drivers
{

IR::IR(std::vector<Sensor> sensors, NRF52Pin &emitter_pin, uint16_t sample_rate)
    : sensors{sensors}, sample_rate{sample_rate}
{
    // Set analog period for emitting
    emitter_pin.setAnalogPeriodUs((1'000'000 / sample_rate) * SAMPLES_PER_FLASH);
    // Half on, half off
    emitter_pin.setAnalogValue(512);

    // Setup measurement timer
    // TODO: Replace with NRF52ADC
    timer = std::make_unique<Timer>([this]() { this->Run(); });
    timer->EveryUs(1'000'000 / sample_rate);
}

void IR::Run()
{
    if (sensors.empty())
        return;

    // First cycle so allocate sample buffers
    if (raw_samples.empty())
    {
        for (auto &sensor : sensors)
        {
            // Initialise raw sample with current analog value
            std::array<IR_SAMPLE_TYPE, IR_SAMPLE_SIZE> samples{};
            samples.fill(sensor.sense_pin.getAnalogValue());
            raw_samples.push_back(samples);
        }
    }

    // Read IR sensors
    for (size_t i{0}; i < sensors.size(); ++i)
    {
        raw_samples[i][idx] = sensors[i].sense_pin.getAnalogValue();
    }

    // Update state
    prev_idx = idx;
    idx = (idx + 1) % IR_SAMPLE_SIZE;
    last_measurement = uBit.timer.getTime();
}

void IR::RunSignalProcessing()
{
    for (size_t i{0}; i < raw_samples.size(); ++i)
    {
        // Run bandpass filtering
        std::array<IR_SAMPLE_TYPE, IR_SAMPLE_SIZE> filtered{};
        std::array<IR_SAMPLE_TYPE, IR_SAMPLE_SIZE> filtered2{};

        const float centre_bandwidth{2.0f};
        Filters::Bandpass(sample_rate / SAMPLES_PER_FLASH, centre_bandwidth, sample_rate, idx,
                          raw_samples[i], filtered);

        // Make all values absolute
        Filters::Abs(filtered);

        // Run bandpass filter
        Filters::Lowpass(1 / (sample_rate / SAMPLES_PER_FLASH), 1 / sample_rate, idx, filtered,
                         filtered2);

        // Just set the output value for now
        // TODO: "Calibration?"
        *sensors[i].value = filtered2[prev_idx];
    }
}

bool IR::IsMeasuring() { return uBit.timer.getTime() - last_measurement < (1000 / sample_rate); }

}; // namespace Firmware::Drivers
