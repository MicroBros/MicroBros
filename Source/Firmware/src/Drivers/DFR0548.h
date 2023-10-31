#pragma once

#include <array>
#include <memory>

#include <I2C.h>

#include <Core/Bitflags.h>

#include "../Timer.h"
#include "../Utils.h"

namespace Firmware::Drivers
{

/*! \brief DFR0548 micro:bit motor driver expansion board driver
 *
 * The DFR0548 uses a PCA9685PW I2C PWM/LED controller board to control the motors from NXP
 *
 *
 * - More info about the board:
 * https://wiki.dfrobot.com/Micro_bit_Driver_Expansion_Board_SKU_DFR0548
 * - Schematics at:
 * https://github.com/Arduinolibrary/Micro_bit_Driver_Expansion_Board/blob/master/Microbit%20Driver%20Expansion%20Board.PDF
 * - PCA9685 datasheet: https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf
 */
class DFR0548
{
public:
    DFR0548(MicroBit &uBit, MicroBitI2C &i2c, bool smooth_output = true,
            uint16_t pca9685_address = 0x40 << 1);

    //! The various motor outputs of the DFR0548 expansion board
    enum class MotorOutput : uint8_t
    {
        M1 = 0, // LED6/LED7 (Left-back)
        M2 = 1, // LED4/LED5 (Left-front)
        M3 = 2, // LED2/LED3 (Right-back)
        M4 = 3  // LED0/LED1 (Right-front)
    };

    //! Minimum needed registers of the PCA9685
    enum class PCA9685Reg : uint8_t
    {
        //! Mode register 1
        MODE1 = 0x00,
        //! LED0 base, used for M4
        LED0_ON_L = 0x06,
        //! LED2 base, used for M3
        LED2_ON_L = 0x0E,
        //! LED4 base, used for M2
        LED4_ON_L = 0x16,
        //! LED6 base, used for M1
        LED6_ON_L = 0x1E,
        //! Prescaler for PWM output frequency
        PRE_SCALE = 0xFE,
    };

    //! Struct for Motor values
    struct MotorValues
    {
        //! Motor 1
        int16_t m1{0};
        //! Motor 2
        int16_t m2{0};
        //! Motor 3
        int16_t m3{0};
        //! Motor 4
        int16_t m4{0};

        // Generate comparison functions
        auto operator<=>(const MotorValues &) const = default;
    };

    // clang-format off
    /*! \brief Bitflags for the PCA9685 MODE1 register
    */
    BITFLAGS_BEGIN(PCA9685Mode1, uint8_t)
        //! Enable restart logic
        RESTART = 1 << 7,
        //! External clock instead of internal
        EXTCLK = 1 << 6,
        //! Register auto-increment
        AI = 1 << 5,
        //! Enable lower power moed (Oscillator off)
        SLEEP = 1 << 4,
        //! Do respond to I2C subaddress 1
        SUB1 = 1 << 3,
        //! Do respond to I2C subaddress 2
        SUB2 = 1 << 2,
        //! Do respond to I2C subaddress 3
        SUB3 = 1 << 1,
        //! Do respond to LED All Call I2C address
        ALLCALL = 1 << 0,
    BITFLAGS_END(PCA9685Mode1)
    // clang-format on

    //! Set the speeds `[-4095, 4095]` on all four motors in a single I2C write
    void SetMotors(int16_t m1_speed, int16_t m2_speed, int16_t m3_speed, int16_t m4_speed);

    //! Stop all the motors
    inline void StopMotors() noexcept { SetMotors(0, 0, 0, 0); }

    //! Array containing all the motor outputs
    constexpr static std::array<MotorOutput, 4> ALL_MOTORS = {MotorOutput::M1, MotorOutput::M2,
                                                              MotorOutput::M3, MotorOutput::M4};

private:
    //! Update the motor values
    void Update();

    MicroBit &uBit;
    MicroBitI2C &i2c;
    uint16_t pca9685_address;
    uint16_t component_id;
    MotorValues current_motors;
    MotorValues set_motors;
    bool smooth_output;
    std::unique_ptr<Firmware::Timer> timer;

    constexpr static std::array<PCA9685Reg, 4> PCA9685_LED_BASE = {
        PCA9685Reg::LED6_ON_L, PCA9685Reg::LED4_ON_L, PCA9685Reg::LED2_ON_L, PCA9685Reg::LED0_ON_L};
};
}; // namespace Firmware::Drivers
