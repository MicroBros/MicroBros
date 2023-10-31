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
    void Run();

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

    bool reverse_forward;

    // Filtering-related variables
    std::deque<float> distance_queue; // Saving latest distance readings for filtering reasons
    static const int FILTER_SIZE = 3; // Number of readings for moving average
    static constexpr float THRESHOLD = 0.05f; // Change in distance threshold
    static const int DEBOUNCE_COUNT = 3;      // Number of continous increases before stopping
    float prevAverageDistance = 0;
    int increasingCount = 0;
    const float MAZE_SIZE = 16.0f;

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
    int16_t fl_pwm;
    int16_t fr_pwm;
    int16_t bl_pwm;
    int16_t br_pwm;

    PID rot_pid;
    PID right_pid;
    PID forward_pid;

    void Initialize();
    void MoveStraight();
    void MoveTurn();
    //! Read the walls and step algorithm
    void StepAlgorithm();
    //! Get global forward that is compensated for reverse forward
    Core::Direction GetGlobalForward();

    void PerpFront();
    void CenterSides();
    void Forward();
    void SetPWM();

    void Turn(char direction);
    void FindMaxima();
    void FindMinima();

    float MovingAverageFilter(float distance);

    void FindWalls();
    void Position();

    //! Returns true if mouse is at a position point
    bool AtPositioningPointB();

    //! Returns true if mouse is at left-right center
    bool IsCenteredLR();
};

} // namespace Firmware
