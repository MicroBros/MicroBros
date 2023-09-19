#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <Core/Bitflags.h>
#include <Core/Maze.h>
#include <Core/Mouse.h>
#include <imgui.h>

#include "Utils.h"

namespace Simulator
{

// clang-format off
/*! \brief Class for a simulator driven Core::Mouse
 *
 *  The SimulatorMaze has to load the maze file and controls the various aspects of the simulated Mouse
 */
class SimulatorMaze
{
public:
    SimulatorMaze(int width, int height);
    explicit SimulatorMaze(std::string path);

    // Simulation
    void Step();
    void Reset();
    void Tick();
    inline void SetAlgorithm(std::optional<std::string> value) { algorithm = value; };
    inline void SetRunning(bool value) { running = value; };
    inline void ToggleRunning() { running = !running; };

    //! Trace using the SimulatorMaze in the Direction, return the hit MazeTile from the Mouse Maze
    Core::MazeTile& TraceTile(Core::Direction direction, int x, int y);
    //! Trace the 3 direction the MicroMouse can see and add it to the Mouse Maze
    void TraceWalls(Core::Direction front_direction, int x, int y);

    // Drawing
    void Draw(Utils::Texture *mouse_sprite);

    // Getters
    inline int Width() { return width; }
    inline int Height() { return height; }
    inline bool IsRunning() { return running; }
    bool IsStepping();
    inline float &Speed() { return speed; }
private:
    uint64_t last_step{0};
    float last_x{0.0};
    float last_y{0.0};
    float last_rot{0.0};
    std::optional<std::string> algorithm{std::nullopt};
    std::unique_ptr<Core::Maze> maze{nullptr};
    std::unique_ptr<Core::Mouse> mouse{nullptr};
    int width, height;
    bool running{false};
    float speed{3.0f};
};

} // namespace Simulator
