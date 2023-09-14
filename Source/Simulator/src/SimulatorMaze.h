#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <Bitflags.h>
#include <imgui.h>

#include "Utils.h"

enum class SimulatorMazeTile
{
    // Sides of maze walls present
    Top = 1 << 0,
    Bottom = 1 << 1,
    Left = 1 << 2,
    Right = 1 << 3,

    // Various special tiles
    Start = 1 << 9,
    Goal = 1 << 10,
};
BITFLAGS(SimulatorMazeTile);

class SimulatorMaze
{
public:
    SimulatorMaze(int width, int height);
    explicit SimulatorMaze(std::string path);
    size_t GetIndex(int x, int y);

    // Simulation
    void Step();
    void Reset();
    inline void SetRunning(bool val)
    {
        running = val;
    };
    inline void ToggleRunning()
    {
        running = !running;
    };
    inline float &Speed()
    {
        return speed;
    }

    // Drawing
    void Draw(Texture *mouse_sprite);

    // Getters
    inline int Width() { return width; }
    inline int Height() { return height; }
    inline bool IsRunning() { return running; }

private:
    std::vector<SimulatorMazeTile> tiles;
    int width, height;
    bool running{false};
    float speed{1.0f};
};
