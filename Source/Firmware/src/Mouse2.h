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

/*! \brief MicroMouse related main loop, state machine with senseing and acting
 */
class Mouse2 : public Core::Mouse
{
public:
    enum class State
    {
        Uninitialized,
        MoveStraight,
        MoveTurn,
        Stopped,
        StepFail
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
    inline void SetRunning(bool running) noexcept
    {
        this->running = running;
        if (running && (state == State::Stopped || state == State::StepFail))
            StepAlgorithm(uBit.timer.getTime());
    }
    //! Return if the mouse is currently finishing a move/step
    inline bool IsMoving() noexcept
    {
        return IsRunning() || (state != State::Stopped && state != State::Uninitialized);
    }
    //! Set algorithm to be used after reset
    inline void SetResetAlgorithm(uint16_t index) noexcept { algorithm = index; }
    //! Get the step iter count
    inline int GetIter() noexcept { return iter; }

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
    CODAL_TIMESTAMP turn_started{0};
    CODAL_TIMESTAMP turn_ended{0};
    CODAL_TIMESTAMP stop_until{0};
    CODAL_TIMESTAMP last_step{0};

    bool reverse_forward;
    int last_forward_heading; // Heading to last forward

    // Filtering-related variables
    Filters::MovingAverageFilter<int, 64> heading_avg;
    Filters::MovingAverageFilter<float, 3> sum_sides_avg;

    //! True if the Mouse2 is running autonomously, set false for manual control
    bool running{false};
    //! Set the algorithm to use on reset, > 0
    int16_t algorithm{-1};

    //! Distance measurements to front, left and right obstructions
    std::atomic<float> f{0.0f};
    std::atomic<float> b{0.0f};
    std::atomic<float> last_f{0.0f};
    std::atomic<float> last_b{0.0f};
    std::atomic<float> l{0.0f};
    std::atomic<float> r{0.0f};
    const float LENGTH_OF_MOUSE = 16;

    int iter = 0;
    int turn_iter{-1};
    bool turn_pid{false};

    uint16_t measurement_interval_ms;

    float forward_pwm = 0.0f;
    float right_pwm = 0.0f;
    float rot_pwm = 0.0f;

    PID right_pid;

    void Initialize(CODAL_TIMESTAMP now);
    void MoveStraight(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt);
    void MoveTurn(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt);
    //! Read the walls and step algorithm
    void StepAlgorithm(CODAL_TIMESTAMP now);
    //! Called with global direction of a move
    void MovedTile(Core::Direction moved_tile);
    //! Get global forward that is compensated for reverse forward
    Core::Direction GetGlobalForward();

    void SetMotors(float forward, float right, float rot);
    //! Calibrate the forward heading
    void CalibrateForward();

#ifdef MOCK
    float &GetDistanceAlias(Core::Direction direction);
    //! Mock sensors
    void MockSensors();
#endif
};

} // namespace Firmware
