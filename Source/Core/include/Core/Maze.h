#pragma once

#include <cstdint>
#include <fmt/format.h>
#include <stddef.h>
#include <stdexcept>
#include <vector>

#include "Bitflags.h"

namespace Core
{

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

//! The various directions the Mouse will face in the Maze
class Direction
{
public:
    using ValueType = uint8_t;

    enum ValueEnum : ValueType
    {
        Up = 0,
        Right = 1,
        Down = 2,
        Left = 3,
    };

    Direction(ValueEnum direction) : direction{direction} {}

    //! Get the closest direction based on rotation in degrees
    static Direction FromRot(float rot);

    //! Turn the Direction right n times
    inline Direction TurnRight(int n) noexcept
    {
        return static_cast<ValueEnum>((static_cast<int>(direction) + n) % Values);
    }
    //! Turn the Direction right 1 time
    inline Direction TurnRight() noexcept { return TurnRight(1); }

    //! Turn the Direction left n times
    inline Direction TurnLeft(int n) noexcept
    {
        auto temp{(static_cast<int>(direction) - n) % Values};
        if (temp < 0)
            temp += Values;
        return static_cast<ValueEnum>(temp);
    }
    //! Turn the Direction left 1 time
    inline Direction TurnLeft() noexcept { return TurnLeft(1); }

    //! Get the Enum value of the Direction
    inline ValueEnum Value() noexcept { return direction; }

    //! Get the MazeTile bitflag for the Direction
    inline MazeTile TileSide() noexcept
    {
        // Lookup table to get tile-side
        static const MazeTile tiles[4] = {MazeTile::Up, MazeTile::Right, MazeTile::Down,
                                          MazeTile::Left};
        return tiles[static_cast<int>(direction)];
    }

    //! Get the string value of the Direction
    std::string_view ToString();

private:
    static const ValueType Values = static_cast<ValueType>(ValueEnum::Left) + 1;
    ValueEnum direction{};
};

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

        return tiles[(width * y) + x];
    }

private:
    int width;
    int height;
    std::vector<MazeTile> tiles;
};

} // namespace Core
