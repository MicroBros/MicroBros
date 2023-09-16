#pragma once

#include <cstdint>
#include <fmt/format.h>
#include <stddef.h>
#include <stdexcept>
#include <vector>

#include "Bitflags.h"

#define MAZE_INDEX(x, y) (width * y) + x

namespace Core
{

// clang-format off
BITFLAGS_BEGIN(MazeTile, uint8_t)
    // Sides of maze walls present
    Top = 1 << 0,
    Bottom = 1 << 1,
    Left = 1 << 2,
    Right = 1 << 3,

    // Various special tiles
    Start = 1 << 6,
    Goal = 1 << 7,
BITFLAGS_END(MazeTile)
// clang-format on

class Maze
{
public:
    Maze(int width, int height);

    inline MazeTile &GetTile(int x, int y)
    {
        if (x >= width)
            throw std::out_of_range(fmt::format("GetTile x {} out of range {} width", x, width));
        if (y >= height)
            throw std::out_of_range(fmt::format("GetTile y {} out of range {} height", y, height));

        return tiles[MAZE_INDEX(x, y)];
    }

private:
    int width;
    int height;
    std::vector<MazeTile> tiles;
};

} // namespace Core
