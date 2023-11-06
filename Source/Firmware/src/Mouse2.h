#pragma once
#include <MicroBit.h>

#include <Core/Mouse.h>

#include <math.h>

#include <deque>

#include "Drivers/DFR0548.h"
#include "Drivers/HCSR04.h"
#include "Drivers/IR.h"
#include "PID.h"
#include "Utils.h"

namespace Firmware
{

class Mouse2 : public Core::Mouse
{
public:
    enum class State
    {
        Uninitialized,
        MoveStraight,
        MoveTurn,
        Stopped,
    };

    Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver);
    //! Run regulation for movement, ran until movement is done
    void Run(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt);

    //! \brief Start a single step
    //!
    //! In short a step is taking a sensor reading in ideal position, doing an algorithm step and
    //! then moving to there for the next ideal sensor reading spot
    void Step();

    //! Reset the Mouse satte
    void Reset();

    //! \brief Get the distance in local \p direction (up=forward, down=backward)
    float GetDistance(Core::Direction direction);

    //! Get if the mouse is running (auto-execute steps)
    inline bool IsRunning() noexcept { return running; }
    //! Set if the mouse is running
    inline void SetRunning(bool running) noexcept { this->running = running; }
    //! Return if the mouse is currently finishing a move/step
    inline bool IsMoving() noexcept { return IsRunning(); }
    //! Set algorithm to be used after reset
    inline void SetResetAlgorithm(uint16_t index) noexcept { algorithm = index; }

private:
    //! External class objects
    MicroBit &uBit;
    Drivers::DFR0548 *driver;
    std::unique_ptr<Drivers::HCSR04> ultrasonics;
    std::unique_ptr<Drivers::IR> IRs;
    int sensor_count;
    uint64_t prev_time_ms; // Value of last time reading
    State state{State::Uninitialized};
    Core::Direction move_direction{Core::Direction::Forward}; // Move direction (local)
    CODAL_TIMESTAMP next_expected_tiley_ms;
    CODAL_TIMESTAMP next_algorithm_step_ms{std::numeric_limits<CODAL_TIMESTAMP>::max()};

    bool reverse_forward;
    int heading;
    int heading_diff; // Heading difference to forward

    // Filtering-related variables
    Filters::MovingAverageFilter<float, 3> sum_sides_avg;

    //! True if the Mouse2 is running autonomously, set false for manual control
    bool running{false};
    //! Set the algorithm to use on reset, > 0
    int16_t algorithm{-1};

    //! Distance measurements to front, left and right obstructions
    float f{0.0f};
    float b{0.0f};
    float l{0.0f};
    float r{0.0f};
    const float LENGTH_OF_MOUSE = 16;

    int iter = 0;

    uint16_t measurement_interval_ms;

    float forward_pwm = 0.0f;
    float right_pwm = 0.0f;
    float rot_pwm = 0.0f;

    PID right_pid;

    void Initialize(CODAL_TIMESTAMP now);
    void MoveStraight(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt);
    void MoveTurn(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt);
    //! Read the walls and step algorithm
    void StepAlgorithm();
    //! Called with global direction of a move
    void MovedTile(Core::Direction moved_tile);
    //! Get global forward that is compensated for reverse forward
    Core::Direction GetGlobalForward();

    void SetMotors(float forward, float right, float rot);
    //! Calibrate the forward heading
    void CalibrateForward();
};

} // namespace Firmware
