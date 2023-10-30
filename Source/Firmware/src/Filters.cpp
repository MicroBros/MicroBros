#include <numbers>

#include <Core/Log.h>

#include "Filters.h"

const float PI2 = 2.0f * std::numbers::pi_v<float>;

namespace Firmware::Filters
{

BandpassFilter::BandpassFilter(DataSource &source, float centre_hz, float bandwidth_hz,
                               float sample_rate, bool deepCopy)
    : EffectFilter(source, deepCopy), centre_hz{centre_hz}, bandwidth_hz{bandwidth_hz},
      sample_rate{sample_rate}, output{*this}
{
}

BandpassFilter::~BandpassFilter() {}

void BandpassFilter::applyEffect(ManagedBuffer inputBuffer, ManagedBuffer outputBuffer, int format)
{
    if (inputBuffer.length() < 1)
        return;

    int bytes_per_sample{DATASTREAM_FORMAT_BYTES_PER_SAMPLE(format)};
    int sample_count{inputBuffer.length() / bytes_per_sample};
    uint8_t *in{inputBuffer.getBytes()};
    uint8_t *out{outputBuffer.getBytes()};

    // Filtering based on Stackoverflow answer https://stackoverflow.com/a/44066316 (CC BY-SA 3.0)
    float x_2{0.0f}; // delayed x, y samples
    float x_1{0.0f};
    float y_1{0.0f};
    float y_2{0.0f};

    const float f{centre_hz / sample_rate};
    const float bw{bandwidth_hz / sample_rate};

    const float R{1 - (3 * bw)};

    const float Rsq{R * R};
    const float cosf2{2 * cosf(PI2 * f)};

    const float K{(1 - R * cosf2 + Rsq) / (2 - cosf2)};

    const float a0{1.0f - K};
    const float a1{2 * (K - R) * cosf2};
    const float a2{Rsq - K};
    const float b1{2 * R * cosf2};
    const float b2{-Rsq};

    for (int i{0}; i < sample_count; i++)
    {
        float in_value{(float)StreamNormalizer::readSample[format](in)};
        float out_value = a0 * in_value + a1 * x_1 + a2 * x_2 + b1 * y_1 + b2 * y_2;

        // shift delayed x, y samples
        x_2 = x_1;
        x_1 = in_value;
        y_2 = y_1;
        y_1 = out_value;

        StreamNormalizer::writeSample[format](out, (int)(out_value * scale));

        in += bytes_per_sample;
        out += bytes_per_sample;
    }
}

AbsoluteFilter::AbsoluteFilter(DataSource &source, bool deepCopy)
    : EffectFilter(source, deepCopy), output{*this}
{
}

AbsoluteFilter::~AbsoluteFilter() {}

void AbsoluteFilter::applyEffect(ManagedBuffer inputBuffer, ManagedBuffer outputBuffer, int format)
{
    if (inputBuffer.length() < 1)
        return;

    int bytes_per_sample{DATASTREAM_FORMAT_BYTES_PER_SAMPLE(format)};
    int sample_count{inputBuffer.length() / bytes_per_sample};
    uint8_t *in{inputBuffer.getBytes()};
    uint8_t *out{outputBuffer.getBytes()};

    for (int i{0}; i < sample_count; i++)
    {
        // Ensure every sample is absolute
        StreamNormalizer::writeSample[format](out,
                                              (int)abs(StreamNormalizer::readSample[format](in)));

        in += bytes_per_sample;
        out += bytes_per_sample;
    }
}

}; // namespace Firmware::Filters
