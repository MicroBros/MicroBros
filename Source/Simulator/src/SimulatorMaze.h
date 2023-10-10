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
    //! Create an empty SimulatorMaze of width and height
    SimulatorMaze(int width, int height);
    //! Create a SimulatorMaze with mazefile loaded from path
    explicit SimulatorMaze(std::string path);

    //! Run a single step with the Simulator
    void Step();
    //! Reset the state of SimulatorMaze and Mouse
    void Reset();
    //! Tick ran by Simulator, this is what calls Step when the Simulator is running
    void Tick();
    //! Set the name of the Algorithm which will be used on Reset
    inline void SetAlgorithm(std::optional<std::string> value) noexcept { algorithm = value; };
    //! Set if the Simulator should be running (auto-step)
    inline void SetRunning(bool value) noexcept { running = value; };
    //! Toogle if the Simulator should be running
    inline void ToggleRunning() noexcept { running = !running; };
    //! Trace using the SimulatorMaze in the Direction, return the hit MazeTile from the Mouse Maze
    Core::MazeTile& TraceTile(Core::Direction direction, int x, int y);
    //! Trace the 3 direction the MicroMouse can see and add it to the Mouse Maze
    void TraceWalls(Core::Direction front_direction, int x, int y);
    //! Draw the maze using the mouse_sprite Texture
    void Draw(Utils::Texture *mouse_sprite);
    //! Get the width of the Maze
    inline int Width() noexcept { return width; }
    //! Get the height of the Maze
    inline int Height() noexcept { return height; }
    //! Get if the simulation is running
    inline bool IsRunning() noexcept { return running; }
    //! Get if the simulation is current interpolating a step
    bool IsStepping();
    //! Get the speed in steps per second
    inline float &Speed() noexcept { return speed; }
    //! Get a pointer to the Mouse
    inline Core::Mouse* GetMouse() noexcept { return mouse.get(); }
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
