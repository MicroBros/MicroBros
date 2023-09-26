#pragma once

namespace Firmware
{

template <typename T> T Clamp(T min, T max, T value)
{
    if (value < min)
    {
        return min;
    }

    else if (value > max)
    {
        return max;
    }

    else
    {
        return value;
    }
}

float Deg2Rad(float num_deg) { return num_deg * (std::numbers::pi / 180); }
float Rad2Deg(float num_rad) { return num_rad * (180 / std::numbers::pi); }
float NormaliseRad(float x)
{
    x = std::fmod(x + std::numbers::pi, 2 * std::numbers::pi);
    if (x < 0)
        x += std::numbers::pi;
    return x - std::numbers::pi;
}
float NormaliseDeg(float x)
{
    x = std::fmod(x + 180, 360);
    if (x < 0)
        x += 180;
    return x - 180;
}

} // namespace Firmware
