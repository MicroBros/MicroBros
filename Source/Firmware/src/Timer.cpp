#include "Timer.h"

extern MicroBit uBit;

namespace Firmware
{

Timer::Timer(std::function<void()> callback) : callback{callback}
{
    // Keep track of the timers
    static uint16_t base_id{0};

    // Setup the messagebus listening
    id = base_id;
    uBit.messageBus.listen(MICROBIT_ID_MICROBROS_TIMER, id, this, &Timer::TimerCallback,
                           MESSAGE_BUS_LISTENER_IMMEDIATE);

    // Increment the generated id
    base_id++;
}

Timer::~Timer() { Reset(); }

void Timer::EveryMs(CODAL_TIMESTAMP interval)
{
    Reset();
    system_timer_event_every(interval, MICROBIT_ID_MICROBROS_TIMER, id);
}

void Timer::EveryUs(CODAL_TIMESTAMP interval)
{
    Reset();
    system_timer_event_every_us(interval, MICROBIT_ID_MICROBROS_TIMER, id);
}

void Timer::AfterMs(CODAL_TIMESTAMP period)
{
    Reset();
    system_timer_event_after(interval, MICROBIT_ID_MICROBROS_TIMER, id);
}

void Timer::AfterUs(CODAL_TIMESTAMP period)
{
    Reset();
    system_timer_event_after_us(interval, MICROBIT_ID_MICROBROS_TIMER, id);
}

//! Reset the timer
void Timer::Reset() { system_timer_cancel_event(MICROBIT_ID_MICROBROS_TIMER, id); }

void Timer::TimerCallback(MicroBitEvent e)
{
    callback();
    // Self-delete useful for oneshot timers
    if (selfdelete)
        delete this;
};

} // namespace Firmware
