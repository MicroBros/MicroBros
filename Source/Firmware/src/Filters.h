#pragma once

#include <MicroBit.h>
#include <deque>
#include <numbers>
#include <span>

namespace Firmware::Filters
{

//! Basic BandpassFilter
class BandpassFilter : public EffectFilter
{
public:
    BandpassFilter(DataSource &source, float centre_hz, float bandwidth_hz, float sample_rate,
                   bool deepCopy = true);
    ~BandpassFilter();

    virtual void applyEffect(ManagedBuffer inputBuffer, ManagedBuffer outputBuffer,
                             int format) override;

    //! Set the value the output is multiplyed by before returning to an int
    inline void SetOutputScale(float scale) { this->scale = scale; }

    //! Output datastream
    DataStream output;

private:
    float centre_hz;
    float bandwidth_hz;
    float sample_rate;
    float scale{1.0f};
};

//! Make all the buffer values to absolute (positive)
class AbsoluteFilter : public EffectFilter
{
public:
    AbsoluteFilter(DataSource &source, bool deepCopy = false);
    ~AbsoluteFilter();

    virtual void applyEffect(ManagedBuffer inputBuffer, ManagedBuffer outputBuffer,
                             int format) override;

    //! Output DataStream
    DataStream output;
};

template <typename T, size_t SIZE> class MovingAverageFilter
{
public:
    MovingAverageFilter() {}

    inline void AddValue(T value)
    {
        while (queue.size() >= SIZE)
            queue.pop_front();

        queue.push_back(value);
    }

    inline T Mean()
    {
        T sum = 0.0;
        for (T d : queue)
            sum += d;

        return sum / queue.size();
    }

    inline T MeanDegrees()
    {
        float x{0.0};
        float y{0.0};

        for (T v : queue)
        {
            x += std::cos(v * std::numbers::pi / 180.0f);
            y += std::sin(v * std::numbers::pi / 180.0f);
        }

        return std::atan2(y, x) * 180.0f / std::numbers::pi;
    }

    inline T AddValueAndMean(T value)
    {
        AddValue(value);
        return Mean();
    }

private:
    std::deque<T> queue;
};

}; // namespace Firmware::Filters
