#include <algorithm>
#include <cmath>

#include "IR.h"

extern MicroBit uBit;

const size_t SAMPLES_PER_FLASH{2};

namespace Firmware::Drivers
{

IR::IRSink::IRSink(DataSource *source, Sensor &sensor) : source{source}, sensor{sensor}
{
    source->connect(*this);
}

int IR::IRSink::pullRequest()
{
    auto buffer{source->pull()};
    auto format{source->getFormat()};

    uint8_t *end = buffer.getBytes() + (buffer.length() - (buffer.length() / 2));
    int value = StreamNormalizer::readSample[format](end);

    // Attempt to normalise the distance
    *sensor.value = std::clamp(
        std::pow(std::max(value - sensor.base, 0.1f), sensor.exp) * sensor.scale, 0.0f, 20.0f);

    return DEVICE_OK;
}

IR::IR(std::vector<Sensor> sensors, NRF52Pin &emitter_pin, uint16_t sample_rate)
    : sensors{sensors}, sample_rate{sample_rate}
{
    // Half on, half off
    emitter_pin.setAnalogValue(512);
    // Set analog period for emitting
    emitter_pin.setAnalogPeriodUs((1'000'000 / sample_rate) * SAMPLES_PER_FLASH);

    // Setup ADCs
    for (auto &sensor : sensors)
    {
        // Setup the ADC channel for sense pin
        auto adc{std::unique_ptr<NRF52ADCChannel>(uBit.adc.getChannel(sensor.sense_pin))};
        adc->requestSampleRate(sample_rate); // Set sample rate
        adc->setGain(0, 1);                  // Minimize gain to get better range
        adc->enable();

        // Add bandpass filter
        auto bandpass{std::make_unique<Filters::BandpassFilter>(
            adc->output, sample_rate / SAMPLES_PER_FLASH, 16.0f, adc->getSampleRate())};
        bandpass->SetOutputScale(16.0f);

        // Add absolute filter
        auto abs{std::make_unique<Filters::AbsoluteFilter>(bandpass->output)};

        // Add lowpass filter
        auto lowpass{std::make_unique<codal::LowPassFilter>(abs->output, 0.01f)};

        data.push_back({.adc_channel = std::move(adc),
                        .bandpass = std::move(bandpass),
                        .abs = std::move(abs),
                        .sink = std::make_unique<IRSink>(lowpass.get(), sensor),
                        .lowpass = std::move(lowpass)});
    }
}

}; // namespace Firmware::Drivers
