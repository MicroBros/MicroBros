#include <MicroBit.h>

#include "DFR0548.h"

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

DFR0548::DFR0548(MicroBit &uBit, MicroBitI2C &i2c, uint16_t pca9685_address)
    : uBit{uBit}, i2c{i2c}, pca9685_address{pca9685_address << 1} // Correct the address for 7-bit
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
}

void DFR0548::SetMotor(MotorOutput motor, int16_t speed)
{
    // Addr + LED (motor) values
    std::array<uint8_t, 1 + 8> buffer{};

    // Find the LED base address for the motor
    buffer[0] = static_cast<uint8_t>(PCA9685_LED_BASE[static_cast<uint8_t>(motor)]);

    // Set the motor "LED" values in the buffer
    WritePWMValues(buffer.data(), 1, speed);

    // Write the values to the PCA9685 over I2C
    i2c.write(pca9685_address, buffer.data(), buffer.size());
}

void DFR0548::SetMotors(int16_t m1_speed, int16_t m2_speed, int16_t m3_speed, int16_t m4_speed)
{
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

}; // namespace Firmware::Drivers