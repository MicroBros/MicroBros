#include <cmath>

#include <MicroBit.h>

#include "DFR0548.h"

#include <Core/Log.h>

namespace Firmware::Drivers
{

//! Frequency must be in [24, 1526] (Hz) range, otherwise will be clamped
uint8_t CalculatePrescaler(uint16_t frequency)
{
    // Clamp the frequency
    if (frequency < 24)
        frequency = 24;
    else if (frequency > 1526)
        frequency = 1526;

    // Calculate the prescaler by Clock / ((Values (12-bit) = 4096) * frequency (Hz))
    return (uint8_t)std::roundf(25'000'000.0f / (4096.0f * frequency));
}

void WritePWMValues(uint8_t *buffer, uint8_t offset, int16_t speed) noexcept
{
    if (speed > 4095)
        speed = 4095;
    else if (speed < -4095)
        speed = -4095;

    // Going forwards
    if (speed > 0)
    {
        // Forward off L
        buffer[offset + 2] = static_cast<uint8_t>(speed & 0xFF);
        // Forward off H
        buffer[offset + 3] = static_cast<uint8_t>(speed >> 8);
    }
    // Going backwards
    else if (speed < 0)
    {
        speed = std::abs(speed);

        // Backward off L
        buffer[offset + 6] = static_cast<uint8_t>(speed & 0xFF);
        // Backward off H
        buffer[offset + 7] = static_cast<uint8_t>(speed >> 8);
    }
}

DFR0548::DFR0548(MicroBit &uBit, MicroBitI2C &i2c, bool smooth_output, uint16_t pca9685_address)
    : uBit{uBit}, i2c{i2c}, pca9685_address{pca9685_address},
      smooth_output{smooth_output} // Correct the address for 7-bit
{
    // Reset the Mode1 register
    i2c.writeRegister(pca9685_address, static_cast<uint8_t>(PCA9685Reg::MODE1), 0x00);

    // Sleep the oscilator so PRE_SCALE can be written to
    i2c.writeRegister(pca9685_address, static_cast<uint8_t>(PCA9685Reg::MODE1),
                      PCA9685Mode1::SLEEP);

    // Set the prescaler to 50Hz
    i2c.writeRegister(pca9685_address, static_cast<uint8_t>(PCA9685Reg::PRE_SCALE),
                      CalculatePrescaler(50));

    // Delay to let osc wake up
    uBit.sleep(5);

    // Remove sleep again and enable restart and auto-increment I2C logic
    i2c.writeRegister(pca9685_address, static_cast<uint8_t>(PCA9685Reg::MODE1),
                      PCA9685Mode1::RESTART | PCA9685Mode1::AI | PCA9685Mode1::ALLCALL);

    i2c.setFrequency(NRF_TWIM_FREQ_250K);

    /*if (smooth_output)
    {
        timer = std::make_unique<Firmware::Timer>([this]() { this->Update(); });
        timer->EveryMs(10);
    }*/

    // Ensure the first stop is run
    current_motors.m1 = -1;

    StopMotors();
}

inline int16_t Smooth(CODAL_TIMESTAMP passed, int16_t from, int16_t to, int16_t step) noexcept
{
    if (to == from)
        return to;
    else if (from != 0 && (to < 0) != (from < 0) && std::abs(from - to) < step)
        return 0;
    else if (passed >= 2)
    {
        if (to > from)
            return std::min(to, static_cast<int16_t>(from + step));
        else
            return std::max(to, static_cast<int16_t>(from - step));
    }
}

void DFR0548::Update()
{
    CODAL_TIMESTAMP now{uBit.timer.getTime()};
    CODAL_TIMESTAMP passed{now - last_update};
    // Check if there is no need for further smoothing
    if (current_motors == set_motors)
        return;

    current_motors.m1 = Smooth(passed, current_motors.m1, set_motors.m1, 1024);
    current_motors.m2 = Smooth(passed, current_motors.m2, set_motors.m2, 1024);
    current_motors.m3 = Smooth(passed, current_motors.m3, set_motors.m3, 1024);
    current_motors.m4 = Smooth(passed, current_motors.m4, set_motors.m4, 1024);

    // Addr + LED (motors) values
    std::array<uint8_t, 1 + (8 * 4)> buffer{};

    // Find the LED base address for LED0
    buffer[0] = static_cast<uint8_t>(PCA9685Reg::LED0_ON_L);

    // Write the PWM values to the buffer
    WritePWMValues(buffer.data(), 1 + (8 * 0), current_motors.m4);
    WritePWMValues(buffer.data(), 1 + (8 * 1), current_motors.m3);
    WritePWMValues(buffer.data(), 1 + (8 * 2), current_motors.m2);
    WritePWMValues(buffer.data(), 1 + (8 * 3), current_motors.m1);

    // Write the values to the PCA9685 over I2C
    i2c.write(pca9685_address, buffer.data(), buffer.size());
    last_update = now;
}

void DFR0548::SetMotors(int16_t m1_speed, int16_t m2_speed, int16_t m3_speed, int16_t m4_speed)
{
    set_motors.m1 = m1_speed;
    set_motors.m2 = m2_speed;
    set_motors.m3 = m3_speed;
    set_motors.m4 = m4_speed;

    if (smooth_output)
    {
        Update();
    }
    else
    {
        // Avoid writing needlessly as I2C writes can hang
        if (set_motors == current_motors)
            return;
        current_motors.m1 = m1_speed;
        current_motors.m2 = m2_speed;
        current_motors.m3 = m3_speed;
        current_motors.m4 = m4_speed;

        // Addr + LED (motors) values
        std::array<uint8_t, 1 + (8 * 4)> buffer{};

        // Find the LED base address for LED0
        buffer[0] = static_cast<uint8_t>(PCA9685Reg::LED0_ON_L);

        // Write the PWM values to the buffer
        WritePWMValues(buffer.data(), 1 + (8 * 0), m4_speed);
        WritePWMValues(buffer.data(), 1 + (8 * 1), m3_speed);
        WritePWMValues(buffer.data(), 1 + (8 * 2), m2_speed);
        WritePWMValues(buffer.data(), 1 + (8 * 3), m1_speed);

        // Write the values to the PCA9685 over I2C
        i2c.write(pca9685_address, buffer.data(), buffer.size());
    }
}

}; // namespace Firmware::Drivers
