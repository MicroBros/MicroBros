#pragma once
#include <MicroBit.h>

namespace Firmware
{

template <typename T> inline T Clamp(T min, T max, T value)
{
    if (value <= min)
    {
        return min;
    }

    else if (value >= max)
    {
        return max;
    }

    else
    {
        return value;
    }
}

inline float Deg2Rad(float num_deg) { return num_deg * (PI / 180); }
inline float Rad2Deg(float num_rad) { return num_rad * (180 / PI); }
inline float NormaliseRad(float x)
{
    x = std::fmod(x + PI, 2 * PI);
    if (x < 0)
        x += PI;
    return x - PI;
}

inline float NormaliseDeg(float x)
{
    x = std::fmod(x + 180, 360);
    if (x < 0)
        x += 180;
    return x - 180;
}

inline int NormaliseDeg(int x)
{
    x = (x + 180) % 360;
    if (x < 0)
        x += 180;
    return x - 180;
}

} // namespace Firmware
