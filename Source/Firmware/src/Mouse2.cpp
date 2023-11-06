#include <algorithm>

#include <Core/Log.h>

#include "Mouse2.h"

namespace Firmware
{

Mouse2::Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver)
    : uBit{uBit}, driver{driver}, right_pid("lr", 0.85, 0, 10)

{
    std::vector<Drivers::HCSR04::Sensor> sensor_pins = {
        {.echo_pin = uBit.io.P13, .trig_pin = uBit.io.P14, .value = &f},
        {.echo_pin = uBit.io.P15, .trig_pin = uBit.io.P16, .value = &b}};

    std::vector<Drivers::IR::Sensor> IR_pins = {
        {.sense_pin = uBit.io.P1, .value = &l, .base = 360, .scale = 0.01f, .exp = 1.181f},
        {.sense_pin = uBit.io.P2, .value = &r, .base = 360, .scale = 0.01f, .exp = 1.181f}};

    sensor_count = sensor_pins.size();
    prev_time_ms = uBit.timer.getTime();
    ultrasonics = std::make_unique<Drivers::HCSR04>(sensor_pins);
    IRs = std::make_unique<Drivers::IR>(IR_pins, uBit.io.P0);
    measurement_interval_ms = ultrasonics->GetMeasurementInterval();

    // Initialise FloodFill as default algorithm
    SetAlgorithm("FloodFill");

    // Make Jonathan happy and let the IR run a few cycles c:
    for (size_t i{0}; i < 8; ++i)
    {
        IRs->RunSignalProcessing();
        fiber_sleep(10);
    }
}

void Mouse2::Run(CODAL_TIMESTAMP now, CODAL_TIMESTAMP dt)
{
    IRs->RunSignalProcessing();

    // LOG_INFO("Left: {}cm, Right: {}cm", l, r);

    // Stop at end
    float forward{GetDistance(Core::Direction::Forward)};
    if (state == State::MoveStraight && forward < 4.5f)
    {
        state = State::Stopped;
    }

    // Step the algorithm if requested
    if (next_algorithm_step_ms > now)
    {
        // Step the algorithm with current sensor data
        StepAlgorithm();
        // Avoid stepping again until another tile change
        next_algorithm_step_ms = std::numeric_limits<CODAL_TIMESTAMP>::max();
    }

    // FSM for movement states
    switch (state)
    {
    case State::Uninitialized:
        Initialize(now);
        break;
    case State::MoveStraight:
        MoveStraight(now, 60);
        break;
    case State::MoveTurn:
        MoveTurn();
        break;
    case State::Stopped:
        driver->StopMotors();
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

    StepAlgorithm();

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
    auto summ{sum_sides_avg.AddValueAndSum(left + right)};
    static auto last_summ{summ};
    static int itty{0};
    if (summ > (last_summ + 0.150f) && (now > next_expected_tiley_ms))
    {
        ++itty;
        LOG_DEBUG("Tileyy={}", itty);
        // Register the tile move
        MovedTile(GetDirection());
        LOG_DEBUG("Moved to tile x:{}, y:{}", static_cast<int>(x), static_cast<int>(y));

        next_algorithm_step_ms =
            now + 200; // step the algorithm after 200 ms so we have passed the notch
        next_expected_tiley_ms = now + 500; // do not expect a tile change the next 500 ms
        fiber_sleep(5);
    }
    last_summ = summ;

    forward_pwm = 1;
    right_pwm = right_pid.Regulate(0, diff, dt);
    // Set rotation too so we end up straight
    rot_pwm = right_pwm * 0.75;

    // Act
    SetMotors(forward_pwm, right_pwm, rot_pwm);
    ++iter;
}

void Mouse2::MoveTurn()
{
    // TODO: Implement turning, use move_direction (which is local)

    LOG_INFO("TODO: Turning");
    driver->StopMotors();
}

void Mouse2::StepAlgorithm()
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
    if (left < 5.0f)
        GetMaze()->GetTile(x, y) |= global_left.TileSide();
    if (right < 5.0f)
        GetMaze()->GetTile(x, y) |= global_right.TileSide();

    // Get the move directions
    auto move_direction_opt{GetAlgorithm()->Step(this, x, y, global_forward)};
    // Stop if no direction given by algorithm
    if (!move_direction_opt.has_value())
    {
        state = State::Stopped;
        return;
    }

    auto move_direction{move_direction_opt.value()};

    // Forward
    if (move_direction == global_forward)
    {
        state = State::MoveStraight;
        move_direction = Core::Direction::Forward;
    }
    // Backwards (reverse forward)
    else if (move_direction == global_backward)
    {
        reverse_forward = !reverse_forward;
        state = State::MoveStraight;
        move_direction = Core::Direction::Forward;
    }
    // Turn left
    else if (move_direction == global_left)
    {
        state = State::MoveTurn;
        move_direction = Core::Direction::Left;
    }
    // Turn right
    else if (move_direction == global_right)
    {
        state = State::MoveTurn;
        move_direction = Core::Direction::Right;
    }
}

float Mouse2::GetDistance(Core::Direction direction)
{
    switch (direction.Value())
    {
    case Core::Direction::Forward:
        return reverse_forward ? b : f;
    case Core::Direction::Back:
        return reverse_forward ? f : b;
    case Core::Direction::Left:
        return reverse_forward ? r : l;
    case Core::Direction::Right:
        return reverse_forward ? l : r;
    };
}

Core::Direction Mouse2::GetGlobalForward()
{
    return Core::Direction::FromRot(reverse_forward ? (rot + 180.0f) : rot);
}

void Mouse2::Step()
{
    // TODO: This will start a step, basically the time from a sensor reading until the mouse is
    // going to do the next one updating the map
    // In short, it is a one tile move
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
    state = State::Uninitialized;
}

void Mouse2::SetMotors(float forward, float right, float rot)
{
    float denominator{std::max(std::abs(forward) + std::abs(right) + std::abs(rot), 1.0f)};

    driver->SetMotors(
        static_cast<int16_t>(((forward_pwm - right_pwm + rot_pwm) / denominator) * 4095.0f),
        static_cast<int16_t>(((forward_pwm + right_pwm + rot_pwm) / denominator) * 4095.0f),
        static_cast<int16_t>(((forward_pwm + right_pwm - rot_pwm) / denominator) * 4095.0f),
        static_cast<int16_t>(((forward_pwm - right_pwm - rot_pwm) / denominator) * 4095.0f));
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

} // namespace Firmware
