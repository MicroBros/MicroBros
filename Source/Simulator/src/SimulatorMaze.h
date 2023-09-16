#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <Bitflags.h>
#include <Maze.h>
#include <imgui.h>

#include "Utils.h"

namespace Simulator
{

class SimulatorMaze
{
public:
    SimulatorMaze(int width, int height);
    explicit SimulatorMaze(std::string path);

    // Simulation
    void Step();
    void Reset();
    inline void SetRunning(bool val) { running = val; };
    inline void ToggleRunning() { running = !running; };
    inline float &Speed() { return speed; }

    // Drawing
    void Draw(Texture *mouse_sprite);

    // Getters
    inline int Width() { return width; }
    inline int Height() { return height; }
    inline bool IsRunning() { return running; }

private:
    std::unique_ptr<Core::Maze> maze{nullptr};
    int width, height;
    bool running{false};
    float speed{1.0f};
};

} // namespace Simulator
