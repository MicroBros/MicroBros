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

//! The various directions the Mouse will face in the Maze
class Direction
{
public:
    enum ValueEnum : uint8_t
    {
        Up = 0,
        Right = 1,
        Down = 2,
        Left = 3,
    };

    Direction(ValueEnum direction) : direction{direction} {}

    //! Turn the Direction right n times
    inline Direction TurnRight(int n) noexcept
    {
        return static_cast<ValueEnum>((static_cast<int>(direction) + n) % Max);
    }
    //! Turn the Direction right 1 time
    inline Direction TurnRight() noexcept { return TurnRight(1); }

    //! Turn the Direction left n times
    inline Direction TurnLeft(int n) noexcept
    {
        return static_cast<ValueEnum>((static_cast<int>(direction) - n) % Max);
    }
    //! Turn the Direction left 1 time
    inline Direction TurnLeft() noexcept { return TurnLeft(1); }

    //! Get the Enum value of the Direction
    inline ValueEnum Value() noexcept { return direction; }

private:
    ValueEnum direction{};
    const int Max = static_cast<int>(ValueEnum::Left) + 1;
};

// clang-format off
/*! \brief Bitflags to hold the state of individual maze tiles
 *
 *  Currently it contains flags for all four directions of walls and if the tile is a start or goal tile
 */
BITFLAGS_BEGIN(MazeTile, uint8_t)
    // Sides of maze walls present
    Up = 1 << 0,
    Right = 1 << 1,
    Down = 1 << 2,
    Left = 1 << 3,

    // Various special tiles
    Start = 1 << 6,
    Goal = 1 << 7,
BITFLAGS_END(MazeTile)
// clang-format on

/*! \brief Grid based structure with every MazeTile
 *
 *  It keeps track of width and height and has a simple function get a tile based on position
 */
class Maze
{
public:
    //! Create a empty new maze with the width and height specified
    Maze(int width, int height);

    //! Reset the walls of the maze
    void ResetWalls();

    //! Check if a tile has a wall to the side
    bool HasWall(int x, int y, Direction direction);

    //! Get a single MazeTile at the x, y position
    inline MazeTile &GetTile(int x, int y)
    {
#ifndef FIRMWARE
        if (x >= width || x < 0)
            throw std::out_of_range(fmt::format("GetTile x {} out of range {} width", x, width));
        if (y >= height || y < 0)
            throw std::out_of_range(fmt::format("GetTile y {} out of range {} height", y, height));
#endif

        return tiles[MAZE_INDEX(x, y)];
    }

private:
    int width;
    int height;
    std::vector<MazeTile> tiles;
};

} // namespace Core
