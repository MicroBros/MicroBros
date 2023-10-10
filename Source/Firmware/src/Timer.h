#pragma once

#include <functional>

#include <MicroBit.h>

namespace Firmware
{

const uint16_t MICROBIT_ID_MICROBROS_TIMER = 70;

/*! \brief Timer object using callbacks abstracting the EventBus

Timer able to do periodic or oneshot timers using callbacks
 */
class Timer
{
public:
    Timer(std::function<void()> callback);

    ~Timer();

    //! Make the timer fire off every \p interval ms
    void EveryMs(CODAL_TIMESTAMP interval);

    //! Make the timer fire off every \p interval us
    void EveryUs(CODAL_TIMESTAMP interval);

    //! Make the oneshot timer fire off once after \p period ms
    void AfterMs(CODAL_TIMESTAMP period);

    //! Make the oneshot timer fire off once after \p period us
    void AfterUs(CODAL_TIMESTAMP period);

    //! True if the timer should self delete after next callback
    inline Timer *SetSelfDelete(bool selfdelete)
    {
        this->selfdelete = selfdelete;
        return this;
    }

    //! Reset the timer
    void Reset();

private:
    uint16_t id;
    uint32_t interval;
    std::function<void()> callback;
    bool selfdelete{false};

    void TimerCallback(MicroBitEvent e);
};
}; // namespace Firmware
