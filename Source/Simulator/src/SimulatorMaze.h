#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <Bitflags.h>
#include <imgui.h>

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

    void Draw();

    // Getters
    inline int Width() { return width; }
    inline int Height() { return height; }

private:
    std::vector<SimulatorMazeTile> tiles;
    int width, height;
};
