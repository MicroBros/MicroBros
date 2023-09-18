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
    inline void SetAlgorithm(std::optional<std::string> value) { algorithm = value; };
    inline void SetRunning(bool value) { running = value; };
    inline void ToggleRunning() { running = !running; };
    inline float &Speed() { return speed; }
    //! Trace using the SimulatorMaze in the Direction, return the hit MazeTile from the Mouse Maze
    Core::MazeTile& TraceTile(Core::Direction direction, int x, int y);

    // Drawing
    void Draw(Utils::Texture *mouse_sprite);

    // Getters
    inline int Width() { return width; }
    inline int Height() { return height; }
    inline bool IsRunning() { return running; }

private:
    std::optional<std::string> algorithm{std::nullopt};
    std::unique_ptr<Core::Maze> maze{nullptr};
    std::unique_ptr<Core::Mouse> mouse{nullptr};
    int width, height;
    bool running{false};
    float speed{1.0f};
};

} // namespace Simulator
