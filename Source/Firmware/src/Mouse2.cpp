#include <Core/Log.h>

#include "Mouse2.h"

namespace Firmware
{

Mouse2::Mouse2(MicroBit &uBit, Drivers::DFR0548 *driver)
    : uBit{uBit}, driver{driver}, forward_pid("f", -15, 0, 0), right_pid("lr", 0.75, 0, 20),
      rot_pid("rot", 0, 0, 20)
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
}

void Mouse2::Run()
{
    IRs->RunSignalProcessing();

    // FSM for movement states
    switch (state)
    {
    case State::Uninitialized:
        Initialize();
        break;
    case State::MoveStraight:
        MoveStraight();
        break;
    case State::MoveTurn:
        MoveTurn();
        break;
    case State::Stopped:
        driver->StopMotors();
        break;
    }
}

void Mouse2::Initialize()
{
    // Ensure all the ultrasonic sensors has been initialized
    fiber_sleep(sensor_count * measurement_interval_ms);

    // Assume that if back distance is longer than front that the robot was placed with reverse
    // front
    reverse_forward = b > f;

    StepAlgorithm();
}

void Mouse2::MoveStraight()
{
    // Debug
    // LOG("Iter:{}", iter);
    // LOG("Distances: f={}\t l={}\t, r={}\n", f, l, r);
    // LOG("DT={}\n", measurement_interval_ms);

    // Sense
    // TODO: Detect when a tile has been barely entered and do StepAlgorithm

    // Think
    Forward();
    PerpFront();
    CenterSides();

    // Act
    SetPWM();
    driver->SetMotors(bl_pwm, fl_pwm, br_pwm, fr_pwm);
    // LOG_INFO("front(US)={:.1f}cm, back(US)={:.1f}cm, left(IR)={:.1f}cm, right(IR)={:.1f}cm", f,
    // b, l, r);
    //
    ++iter;

    // LOG_INFO("RUN TIME={}", uBit.timer.getTime() - start_time);
    // start_time = uBit.timer.getTime();
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

void Mouse2::SetPWM()
{
    float denominator{
        std::max(std::abs(forward_pwm) + std::abs(right_pwm) + std::abs(rot_pwm), 1.0f)};
    fr_pwm = static_cast<int16_t>(((forward_pwm - right_pwm - rot_pwm) / denominator) * 4095.0f);
    br_pwm = static_cast<int16_t>(((forward_pwm + right_pwm - rot_pwm) / denominator) * 4095.0f);
    fl_pwm = static_cast<int16_t>(((forward_pwm + right_pwm + rot_pwm) / denominator) * 4095.0f);
    bl_pwm = static_cast<int16_t>(((forward_pwm - right_pwm + rot_pwm) / denominator) * 4095.0f);
    // LOG("SetPWM(1): forward_pwm={}\tright_pwm={}\trot_pwm={}\tdenominator={}\n", forward_pwm,
    // right_pwm, rot_pwm, denominator);
    // LOG("SetPWM(2): fr={}\tbr={}\tfl={}\tbl={}\n", fr_pwm, br_pwm, fl_pwm, bl_pwm);
}

void Mouse2::Forward()
{
    float target = 10.0;
    forward_pwm = 1;
    // forward_pwm = forward_pid.Regulate(target, f, measurement_interval_ms);
    // forward_pid.Debug();
}

// Measures diff between left and right readings, sets right_pwm
void Mouse2::CenterSides()
{
    float diff = std::fmod(l - r, MAZE_SIZE);
    right_pwm = right_pid.Regulate(0, diff, measurement_interval_ms);
    right_pid.Debug();
}

void Mouse2::PerpFront()
{
    float diff = std::fmod(l - r, MAZE_SIZE);
    rot_pwm = rot_pid.Regulate(l, r, measurement_interval_ms);
    rot_pid.Debug();
}

void Mouse2::Turn(char direction)
{
    if (direction == 'r')
    {
        fr_pwm = -2048;
        br_pwm = -2048;
        fl_pwm = 2048;
        bl_pwm = 2048;
    }
    else
    {
        fr_pwm = 2048;
        br_pwm = 2048;
        fl_pwm = -2048;
        bl_pwm = -2048;
    }

    SetPWM();
    driver->SetMotors(bl_pwm, fl_pwm, br_pwm, fr_pwm);
    FindMaxima();
    FindMinima();
}

void Mouse2::FindMaxima()
{

    prevAverageDistance = f;
    while (true)
    {
        float avg_distance = MovingAverageFilter(f);
        if (avg_distance < prevAverageDistance + THRESHOLD)
        {
            ++increasingCount;
            if (increasingCount + DEBOUNCE_COUNT)
            {
                break;
            }
        }
        else
        {
            increasingCount = 0;
        }
        prevAverageDistance = 0;
    }
}

void Mouse2::FindMinima()
{

    prevAverageDistance = f;

    // End phase that breaks when shortest distance (perp angle) is found
    while (true)
    {
        float avg_distance = MovingAverageFilter(f);
        if (avg_distance > prevAverageDistance + THRESHOLD)
        {
            ++increasingCount;
            if (increasingCount + DEBOUNCE_COUNT)
            {
                break;
            }
        }
        else
        {
            increasingCount = 0;
        }
        prevAverageDistance = 0;
    }
}

float Mouse2::MovingAverageFilter(float distance)
{
    // Time check to ensure a new distance value is registered before proceeding
    if (uBit.timer.getTime() - prev_time_ms >= measurement_interval_ms * sensor_count)
    {
        if (distance_queue.size() == FILTER_SIZE)
        {
            distance_queue.pop_front();
        }
        distance_queue.push_back(distance);

        float sum = 0.0;
        for (float d : distance_queue)
        {
            sum += d;
        }
        return sum / distance_queue.size();
    }
    prev_time_ms = uBit.timer.getTime();
}

void Mouse2::FindWalls()
{
    auto f_index = (int)(f / MAZE_SIZE);
    auto l_index = (int)(l / MAZE_SIZE);
    auto r_index = (int)(r / MAZE_SIZE);

    Core::Direction front{Core::Direction::FromRot(rot)};
    auto left{front.TurnLeft()};
    auto right{front.TurnRight()};

    GetMaze()->GetTileAdjacent(x, y, front, f_index) |= front.TileSide();
    GetMaze()->GetTileAdjacent(x, y, left, l_index) |= left.TileSide();
    GetMaze()->GetTileAdjacent(x, y, right, r_index) |= right.TileSide();
}

void Mouse2::Position()
{
    // Should determine the relative position of mouse and add to the absolute position
    // Must be called before a new turn
    if (AtPositioningPointB() && IsCenteredLR())
    {
        return;
    }
}

bool Mouse2::AtPositioningPointB()
{
    float THRESHOLD = 1.0f;
    auto disty = std::fmod(b + (LENGTH_OF_MOUSE / 2), MAZE_SIZE);
    if (disty <= THRESHOLD || disty >= MAZE_SIZE - THRESHOLD)
    {
        return true;
    }
    return false;
}

bool Mouse2::IsCenteredLR()
{
    float LR_THRESHOLD = 1.0f;
    float diff = std::fmod(l, MAZE_SIZE) - std::fmod(r, MAZE_SIZE);
    if (fabs(diff) <= LR_THRESHOLD)
    {
        return true;
    }
    return false;
}

} // namespace Firmware
