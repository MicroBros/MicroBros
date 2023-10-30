#pragma once

#include <span>

namespace Firmware::Filters
{

//! Run band pass filtering on a buffer
void Bandpass(float centre_hz, float bandwidth_hz, int sample_rate, size_t idx,
              const std::span<float> input, std::span<float> output);

//! Make all values of buffer absolute
void Abs(std::span<float> buffer);

//! Basic lowpass filter
void Lowpass(float dt, float rc, size_t idx, const std::span<float> input, std::span<float> output);

}; // namespace Firmware::Filters
