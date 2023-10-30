#pragma once

#include <MicroBit.h>
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

}; // namespace Firmware::Filters
