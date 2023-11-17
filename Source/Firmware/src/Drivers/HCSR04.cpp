#include "HCSR04.h"

#include <algorithm>

extern MicroBit uBit;

namespace Firmware::Drivers
{

HCSR04::HCSR04(std::vector<Sensor> sensors, uint16_t measurement_interval)
    : sensors{sensors}, measurement_interval{measurement_interval}
{
    // Setup echo pin event handling
    for (auto &sensor : sensors)
    {
        sensor.echo_pin.eventOn(DEVICE_PIN_EVENT_ON_PULSE);
        uBit.messageBus.listen(sensor.echo_pin.id, DEVICE_PIN_EVT_PULSE_HI, this, &HCSR04::OnPulse,
                               MESSAGE_BUS_LISTENER_IMMEDIATE);
        last_measurements.push_back(0);
    }

    // Setup trigger timer
    timer = std::make_unique<Timer>([this]() { this->Run(); });
    timer->EveryMs(measurement_interval);
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
        // Formula is us/58 per datasheets, clamp between 0-399cm anything over, assume 0 (blocked)
        float dist{us / 58.0f};
        if (dist > 399.0f)
            dist = 0.0f;
        if (sensors[i].last_value)
            sensors[i].last_value->store(sensors[i].value->load());

        sensors[i].value->store(dist); // sensors[i].filter.AddValueAndMean(dist);

        last_measurements[i] = uBit.timer.getTime();
    }
}

bool HCSR04::IsMeasuring()
{
    auto time{uBit.timer.getTime()};

    for (auto last_measurement : last_measurements)
    {
        // A sensor has failed to measure
        if (time - last_measurement > measurement_interval * sensors.size())
            return false;
    }

    return true;
}

}; // namespace Firmware::Drivers
