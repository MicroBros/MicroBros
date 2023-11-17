#include <algorithm>

#include <Core/Log.h>

// #define MOCK // (Mock sensors, useful to test state machine without a working robot)
#include "Mouse2.h"

namespace Firmware
{

Mouse2::Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver)
    : uBit{uBit}, driver{driver}, right_pid("r", 0.85, 0, 10)

{
    std::vector<Drivers::HCSR04::Sensor> sensor_pins = {
        {.echo_pin = uBit.io.P13, .trig_pin = uBit.io.P14, .value = &f},
        {.echo_pin = uBit.io.P15, .trig_pin = uBit.io.P16, .value = &b}};

    std::vector<Drivers::IR::Sensor> IR_pins = {
        {.sense_pin = uBit.io.P1, .value = &l, .base = 360, .scale = 0.01f, .exp = 1.181f},
        {.sense_pin = uBit.io.P2, .value = &r, .base = 360, .scale = 0.01f, .exp = 1.181f}};

    sensor_count = sensor_pins.size();
    prev_time_ms = uBit.timer.getTime();
    ultrasonics = std::make_unique<Drivers::HCSR04>(sensor_pins, 24);
    IRs = std::make_unique<Drivers::IR>(IR_pins, uBit.io.P0);
    measurement_interval_ms = ultrasonics->GetMeasurementInterval();

    // Initialise WallFollower as default algorithm
    SetAlgorithm("WallFollower");
    // SetAlgorithm("FloodFill");

    // Make Jonathan happy and let the IR run a few cycles c:
    fiber_sleep(80);
}

void Mouse2::Run(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt)
{
#ifdef MOCK
    // Mock sensors
    MockSensors();
#endif

    // Run IR sensors
    fiber_sleep(1);

    // heading_avg.AddValue(NormaliseDeg(uBit.compass.heading()));

    /*{
        // Seed headings
        for (size_t i{0}; i < 32; ++i)
        {
            heading_avg.AddValue(NormaliseDeg(uBit.compass.heading()));
            fiber_sleep(2);
        }

        // Set forward
        CalibrateForward();

        while (true)
        {
            for (size_t i{0}; i < 32; ++i)
            {
                heading_avg.AddValue(NormaliseDeg(uBit.compass.heading()));
                fiber_sleep(2);
            }

            int heading{heading_avg.MeanDegrees()};
            int heading_diff{NormaliseDeg(heading - last_forward_heading)};
            LOG_INFO("Heading: {}, diff: {}", heading, heading_diff);
        }
    }*/

    // LOG_INFO("Left: {}cm, Right: {}cm", l, r);

    // Step the algorithm if requested
    if (((now > next_algorithm_step_ms && state != State::Stopped) ||
         (state == State::MoveStraight && GetDistance(Core::Direction::Forward) < 3.5f)) &&
        IsMoving())
    {
        // Avoid stepping again until another tile change
        next_algorithm_step_ms = std::numeric_limits<CODAL_TIMESTAMP>::max();
        // Step the algorithm with current sensor data
        StepAlgorithm(now);

#ifdef MOCK
        // Just force move for mocking
        if (state != State::Stopped)
            MovedTile(GetGlobalForward());
#endif
    }

    // FSM for movement states
    switch (state)
    {
    case State::Uninitialized:
        // Only initialize if running
        if (IsRunning())
            Initialize(now);
        break;
    case State::MoveStraight:
        MoveStraight(now, 60);
        break;
    case State::MoveTurn:
        MoveTurn(now, 60);
        break;
    case State::Stopped:
        driver->StopMotors();
        break;
    default:
        driver->StopMotors();
        LOG_ERROR("INVALID STATE");
        break;
    }

    // fiber_sleep(10);
}

void Mouse2::Initialize(CODAL_TIMESTAMP now)
{
    // Ensure all the ultrasonic sensors has been initialized
    fiber_sleep(sensor_count * measurement_interval_ms);

    // Assume that if back distance is longer than front that the robot was placed with reverse
    // front
    reverse_forward = b > f;

    StepAlgorithm(now);

    next_expected_tiley_ms = now + 250; // Assume next tiley is at least 100ms later after start
}

void Mouse2::MoveStraight(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt)
{
    // TODO: Detect when a tile has been barely entered and do StepAlgorithm
    float left{GetDistance(Core::Direction::Left)};
    float right{GetDistance(Core::Direction::Right)};

    // Correct left and right in case there is no wall present
    if (left > 5.5f)
        left = 16.0f - 7.8f - right;
    if (right > 5.5f)
        right = 16.0f - 7.8f - left;

    float diff{left - right};

    // Detect tile changes by seeing a difference in IR values
    auto summ{sum_sides_avg.AddValueAndMean(left + right)};
    static auto last_summ{summ};
    static int itty{0};
    if (summ > (last_summ + 0.150f) && (now > next_expected_tiley_ms))
    {
        ++itty;
        LOG_DEBUG("Tileyy={}", itty);
        // Register the tile move
        MovedTile(GetGlobalForward());
        LOG_DEBUG("Moved to tile x:{}, y:{}", static_cast<int>(x), static_cast<int>(y));

        next_algorithm_step_ms =
            now + 200; // step the algorithm after 200 ms so we have passed the notch
        next_expected_tiley_ms = now + 500; // do not expect a tile change the next 500 ms
        fiber_sleep(5);
    }
    last_summ = summ;

    forward_pwm = 0.8f;
    right_pwm = right_pid.Regulate(0, diff, dt);
    // rot_pwm = rot_pid.Regulate(0, diff, dt);
    //   Set rotation too so we end up straight

    rot_pwm = right_pwm * 0.85;

    // Act
    SetMotors(forward_pwm, right_pwm, rot_pwm);
}

void Mouse2::MoveTurn(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt)
{
    // Get the current heading in degrees
    /*int heading{heading_avg.MeanDegrees()};
    int heading_diff{NormaliseDeg(heading - last_forward_heading)};
    LOG_DEBUG("heading_diff:{}, heading:{}, lfheading:{}", heading_diff, heading,
              last_forward_heading);
    fiber_sleep(10);

    // End the turn after having turned ~85 degrees
    if (heading_diff > 60 || heading_diff < -60)
    {
        // TODO: Do algorithm step
        state = State::MoveStraight;
    }*/

    // TODO: Implement turning, use move_direction (which is local)
    float left{GetDistance(Core::Direction::Left)};
    float right{GetDistance(Core::Direction::Right)};

    // Correct left and right in case there is no wall present
    /*
    if (move_direction == Core::Direction::Right)
        left = 16.0f - 7.8f - right;
    else
        right = 16.0f - 7.8f - left;

    float diff{left - right};
    */
    float turning{move_direction == Core::Direction::Left ? -1.0f : 1.0f};

    CODAL_TIMESTAMP turn_time{now - turn_started};

    // if (now - turn_started > 750)
    if (turn_time > 900)
    //&&
    //  (GetDistance(Core::Direction::Forward) < 12.0f || left < 4.9f || right < 4.9f)) ||
    // turn_time > 950)
    {
        // MovedTile(GetGlobalForward());
        next_algorithm_step_ms = now + (turn_time * 0.9);
        next_expected_tiley_ms = now + 450;
        state = State::MoveStraight;
    }
    else
    {
        float time{std::clamp(turn_time / 800.0f, 0.0f, 1.0f)};
        // The most cursed math
        float forward{0.45f * (0.2f + 0.8f * (1.0f - time))};
        float right{turn_time > 600 ? -turning * 0.45f : turning * 0.40f * (1.0f - time)};
        float rotation{turning * (0.6f + 0.4f * (1.0f - time))};

        SetMotors(forward, right, rotation);
    }
    // Middle of turn
    /*else if (turn_time > 200 && turn_time < 450)
    {
        SetMotors(0.5f, -turning * 0.1f, turning * 0.5f);
    }
    // Start / end of turn
    else
    {
        SetMotors(turn_time > 450 ? 0.00f : 0.10f, turning * 0.1f, turning * 1.0f);
    }*/
}

void Mouse2::StepAlgorithm(CODAL_TIMESTAMP now)
{
    // Get distances to sides
    float front{GetDistance(Core::Direction::Forward)};
    float left{GetDistance(Core::Direction::Left)};
    float right{GetDistance(Core::Direction::Right)};

    // Get the global direction for local forward and global sides
    auto global_forward{GetGlobalForward()};
    auto global_left{global_forward.TurnLeft()};
    auto global_right{global_forward.TurnRight()};
    auto global_backward{global_forward.TurnRight(2)};

    // Sense walls
    if (front < 20.0f)
        GetMaze()->GetTile(x, y) |= global_forward.TileSide();
    else
        GetMaze()->GetTile(x, y) &= ~global_forward.TileSide();

    if (left < 5.0f)
        GetMaze()->GetTile(x, y) |= global_left.TileSide();
    else
        GetMaze()->GetTile(x, y) &= ~global_left.TileSide();

    if (right < 5.0f)
        GetMaze()->GetTile(x, y) |= global_right.TileSide();
    else
        GetMaze()->GetTile(x, y) &= ~global_right.TileSide();

    LOG_DEBUG("Stepping algorithm, current forward(global): {}, x:{} y:{} rot:{}",
              global_forward.ToString(), static_cast<int>(x), static_cast<int>(y),
              static_cast<int>(rot));

    // Get the move directions
    auto move_direction_opt{GetAlgorithm()->Step(this, x, y, global_forward)};
    // Stop if no direction given by algorithm
    if (!move_direction_opt.has_value())
    {
        LOG_ERROR("No move direction found my algorithm, stopping!");
        state = State::Stopped;
        return;
    }

    LOG_DEBUG("Next forward(global): {}", move_direction_opt.value().ToString());

    auto dir{move_direction_opt.value()};

    // Forward
    if (dir == global_forward)
    {
        state = State::MoveStraight;
        move_direction = Core::Direction::Forward;
        LOG_DEBUG("Move forward");
    }
    // Backwards (reverse forward)
    else if (dir == global_backward)
    {
        reverse_forward = !reverse_forward;
        state = State::MoveStraight;
        move_direction = Core::Direction::Forward;
        LOG_DEBUG("Move forward & reverse");
    }
    // Turn left
    else if (dir == global_left)
    {
        state = State::MoveTurn;
        move_direction = Core::Direction::Left;
        turn_started = now;
        LOG_DEBUG("Turn left");
    }
    // Turn right
    else if (dir == global_right)
    {
        state = State::MoveTurn;
        move_direction = Core::Direction::Right;
        turn_started = now;
        LOG_DEBUG("Turn right");
    }

    rot = dir.Degrees();
    iter++;

#ifdef MOCK
    next_algorithm_step_ms = now + 500; // Force next step in 500ms
#endif
}

float Mouse2::GetDistance(Core::Direction direction)
{
    switch (direction.Value())
    {
    case Core::Direction::Forward:
        return reverse_forward ? b : f;
    case Core::Direction::Backward:
        return reverse_forward ? f : b;
    case Core::Direction::Left:
        return reverse_forward ? r : l;
    case Core::Direction::Right:
        return reverse_forward ? l : r;
    default:
        return 0;
    };
}

Core::Direction Mouse2::GetGlobalForward() { return Core::Direction::FromRot(rot); }

void Mouse2::Step()
{
    // Return early if already moving
    if (IsMoving())
        return;

#ifdef MOCK
    // Mock sensors
    MockSensors();
#endif

    // Step manually
    StepAlgorithm(uBit.timer.getTime());

#ifdef MOCK
    // Just force move for mocking
    if (state != State::Stopped)
        MovedTile(GetGlobalForward());
    state = State::Stopped;
#endif
}

void Mouse2::Reset()
{
    Mouse::Reset();

    // Update algorithm if set manually
    if (algorithm >= 0)
    {
        if (!SetAlgorithm(algorithm))
            LOG_ERROR("Error setting algorithm: {}", algorithm);
    }

    // Reset state
    SetMotors(0, 0, 0);
    state = State::Uninitialized;
    iter = 0; // Reset maze for MouseService
}

void Mouse2::SetMotors(float forward, float right, float rot)
{
    float denominator{std::max(std::abs(forward) + std::abs(right) + std::abs(rot), 1.0f)};

    // Reverse set motors if reverse forward
    if (reverse_forward)
    {
        forward *= -1;
        right *= -1;
        // rot *= -1;
    }

    driver->SetMotors(static_cast<int16_t>((forward - right + rot) / denominator * 4095.0f),
                      static_cast<int16_t>((forward + right + rot) / denominator * 4095.0f),
                      static_cast<int16_t>((forward + right - rot) / denominator * 4095.0f),
                      static_cast<int16_t>((forward - right - rot) / denominator * 4095.0f));
}

void Mouse2::MovedTile(Core::Direction direction)
{
    switch (direction.Value())
    {
    case Core::Direction::Up:
        y++;
        break;
    case Core::Direction::Right:
        x++;
        break;
    case Core::Direction::Down:
        y--;
        break;
    case Core::Direction::Left:
        x--;
        break;
    }
    // Clamp em for now
    x = std::clamp(x, 0.0f, 15.0f);
    y = std::clamp(y, 0.0f, 15.0f);
}

void Mouse2::CalibrateForward() { last_forward_heading = heading_avg.MeanDegrees(); }

#ifdef MOCK

float &Mouse2::GetDistanceAlias(Core::Direction direction)
{
    switch (direction.Value())
    {
    case Core::Direction::Forward:
        return reverse_forward ? b : f;
    case Core::Direction::Backward:
        return reverse_forward ? f : b;
    case Core::Direction::Left:
        return reverse_forward ? r : l;
    case Core::Direction::Right:
        return reverse_forward ? l : r;
    };
}

void Mouse2::MockSensors()
{
    if (y > 4.9f && x < 1.0f)
    {
        GetDistanceAlias(Core::Direction::Forward) = 8.0f;
        GetDistanceAlias(Core::Direction::Right) = 6.1f;
        GetDistanceAlias(Core::Direction::Left) = 4.1f;
    }
    else if (std::abs(y - 5) < 0.1f && x > 7.9f)
    {
        GetDistanceAlias(Core::Direction::Forward) = 8.0f;
        GetDistanceAlias(Core::Direction::Right) = 4.1f;
        GetDistanceAlias(Core::Direction::Left) = 6.1f;
    }
    else
    {
        GetDistanceAlias(Core::Direction::Forward) = 21.0f;
        GetDistanceAlias(Core::Direction::Right) = 4.1f;
        GetDistanceAlias(Core::Direction::Left) = 4.1f;
    }

    GetDistanceAlias(Core::Direction::Backward) = 4.0f;
}

#endif

} // namespace Firmware
