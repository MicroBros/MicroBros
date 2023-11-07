#pragma once
#include <MicroBit.h>
#include <array>
#include <atomic>
#include <memory>

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

template <size_t SIZE> class CircularSink : public codal::DataSink
{
public:
    CircularSink(DataSource *source, size_t nth = 1)
        : source{source}, nth{std::max(nth, static_cast<size_t>(1))}
    {
        source->connect(*this);
    }

    inline int pullRequest()
    {
        // Let nth paramter make CircularSink only pull every nth parameter
        i++;
        if ((i % nth) != 0)
        {
            return DEVICE_OK;
        }

        size_t next{(index + 1) % SIZE};

        // Make shared_ptr
        buffers[next] = source->pull();

        index = next;

        return DEVICE_OK;
    }

    inline ManagedBuffer &GetBuffer() { return buffers[index]; }

private:
    codal::DataSource *source;
    std::array<ManagedBuffer, SIZE> buffers;
    std::atomic<size_t> index{0};
    size_t nth;
    size_t i{0};
};

} // namespace Firmware
