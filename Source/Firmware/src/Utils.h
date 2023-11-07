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
        x += 360;
    return x - 180;
}

inline int NormaliseDeg(int x)
{
    x = (x + 180) % 360;
    if (x < 0)
        x += 360;
    return x - 180;
}

// Assumes parameters are in range [-180, 180]
// Returns shortest diff range with alpha in focus
inline int ShortestDeg(int alpha, int beta) { int diff = alpha - beta; }

} // namespace Firmware
