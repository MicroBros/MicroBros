#include <numbers>

#include <Core/Log.h>

#include "Filters.h"

namespace Firmware::Filters
{

const float PI2 = 2.0f * std::numbers::pi_v<float>;

void Bandpass(float centre_hz, float bandwidth_hz, int sample_rate, size_t idx,
              const std::span<float> input, std::span<float> output)
{
    if (input.size() != output.size())
    {
        LOG_ERROR("Bandpass Input/output size nonmatching, {} != {}", input.size(), output.size());
        return;
    }

    size_t n{input.size()};

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

    for (size_t i{0}; i < n; ++i)
    {
        // Relative to start of sampling
        size_t i2{(idx + i) % n};

        // IIR difference equation
        output[i2] = a0 * input[i2] + a1 * x_1 + a2 * x_2 + b1 * y_1 + b2 * y_2;

        // shift delayed x, y samples
        x_2 = x_1;
        x_1 = input[i2];
        y_2 = y_1;
        y_1 = output[i2];
    }
}

void Abs(std::span<float> buffer)
{
    for (size_t i{0}; i < buffer.size(); ++i)
    {
        buffer[i] = fabsf(buffer[i]);
    }
}

// Simple infinite impulse response filter implemented based on Wikipedia pseudocode
void Lowpass(float dt, float rc, size_t idx, const std::span<float> input, std::span<float> output)
{
    if (input.size() != output.size())
    {
        LOG_ERROR("Lowpass Input/output size nonmatching, {} != {}", input.size(), output.size());
        return;
    }

    size_t n{input.size()};
    float a{dt / (rc + dt)};

    output[idx] = input[idx];
    for (size_t i{1}; i < n; ++i)
    {
        size_t out_idx{(idx + i) % n};
        size_t prev_idx{(idx + i - 1) % n};
        output[out_idx] = output[prev_idx] + a * (input[out_idx] - output[prev_idx]);
    }
}

}; // namespace Firmware::Filters
