#include "HCSR04.h"

#include <algorithm>

extern MicroBit uBit;

namespace Firmware::Drivers
{

HCSR04::HCSR04(std::vector<Sensor> sensors, uint16_t measurement_interval)
    : sensors{sensors}, measurement_interval{measurement_interval}
{
    // Setup trigger timer
    timer = std::make_unique<Timer>([this]() { this->Run(); });
    timer->EveryMs(measurement_interval);

    // Setup echo pin event handling
    for (auto &sensor : sensors)
    {
        sensor.echo_pin.eventOn(DEVICE_PIN_EVENT_ON_PULSE);
        uBit.messageBus.listen(sensor.echo_pin.id, DEVICE_PIN_EVT_PULSE_HI, this, &HCSR04::OnPulse,
                               MESSAGE_BUS_LISTENER_IMMEDIATE);
    }
}

void HCSR04::Run()
{
    if (sensors.empty())
        return;
    // Get ready for the next sensor
    idx = (idx + 1) % sensors.size();
    // Set high (trig)
    sensors[idx].trig_pin.setDigitalValue(true);
    // Wait for 10us
    system_timer_wait_us(10);
    // Set low (trig)
    sensors[idx].trig_pin.setDigitalValue(false);
}

void HCSR04::OnPulse(Event event)
{
    for (size_t i{0}; i < sensors.size(); ++i)
    {
        auto &sensor{sensors[i]};

        if (sensor.echo_pin.id != event.source)
            continue;
        //  Duration of the pulse in us
        uint64_t us = event.timestamp;
        // Formula is us/58 per datasheets, clamp between 2-400cm
        *(sensors[i].value) = std::clamp(us / 58.0f, 2.0f, 400.0f);

        last_measurement = uBit.timer.getTime();
    }
}

bool HCSR04::IsMeasuring()
{
    return uBit.timer.getTime() - last_measurement < measurement_interval;
}

}; // namespace Firmware::Drivers
