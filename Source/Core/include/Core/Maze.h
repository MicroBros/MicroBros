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
enum class Direction : uint8_t
{
    Up,
    Right,
    Down,
    Left,
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
