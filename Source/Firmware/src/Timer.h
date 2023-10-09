#pragma once

#include <functional>

#include <MicroBit.h>

extern MicroBit uBit;

namespace Firmware
{

const uint16_t MICROBIT_ID_MICROBROS_TIMER = 70;

/*! \brief Timer object using callbacks abstracting the EventBus

Timer able to do periodic or oneshot timers using callbacks
 */
class Timer
{
public:
    Timer(std::function<void()> callback, bool oneshot = false)
        : callback{callback}, oneshot{oneshot}
    {
        // Keep track of the timers
        static uint16_t base_id{0};

        // Setup the messagebus listening
        id = base_id;
        uBit.messageBus.listen(MICROBIT_ID_MICROBROS_TIMER, id, this, &Timer::TimerCallback);

        // Increment the generated id
        base_id++;
    }

    ~Timer() { Reset(); }

    //! Make the timer fire off every \p interval ms
    inline void EveryMs(uint64_t interval)
    {
        Reset();
        system_timer_event_every(interval, MICROBIT_ID_MICROBROS_TIMER, id);
    }

    //! Make the timer fire off every \p interval us
    inline void EveryUs(uint64_t interval)
    {
        Reset();
        system_timer_event_every_us(interval, MICROBIT_ID_MICROBROS_TIMER, id);
    }

    //! Reset the timer
    inline void Reset() { system_timer_cancel_event(MICROBIT_ID_MICROBROS_TIMER, id); }

    //! Override if the timer is a oneshot
    inline void SetOneshot(bool oneshot) { this->oneshot = oneshot; }

private:
    uint16_t id;
    uint32_t interval;
    std::function<void()> callback;
    bool oneshot;

    inline void TimerCallback(MicroBitEvent e)
    {
        callback();
        if (oneshot)
            Reset();
    }
};

}; // namespace Firmware
