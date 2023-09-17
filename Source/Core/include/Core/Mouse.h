#pragma once

#include <memory>

#include "Algorithm.h"
#include "Maze.h"

namespace Core
{

/*! \brief Main structure to hold high-level state for the MicroMouse
 *
 *  It contains information about the maze, position and rotation.
 *
 *  The position of the mouse is stored using floats, it is assumed that 0.0, 0.0 is the middle of
 * tile 0,0. Rotation is also stored as a float with 0 being assumed as the starting position
 * pointing upwards/north relative to the maze
 */
class Mouse
{
public:
    //! Initialise the Mouse with a new maze
    Mouse(int width = 16, int height = 16, bool initialize_tiles = true);
    //! Initialise the Mouse with an existing Maze, useful for the Simulator
    Mouse(std::unique_ptr<Maze> maze);

    //! Reset the state of the mouse
    void Reset();

    //! Set the Algorithm the Mouse will use
    inline void SetAlgorithm(std::unique_ptr<Algorithm> value) noexcept
    {
        algorithm = std::move(value);
    }

    /* Getters */
    //! Get the Maze for the Mouse
    inline Maze *GetMaze() noexcept { return maze.get(); }
    //! Get the X-position aligned to middle of tile
    inline float X() noexcept { return x; }
    //! Get the Y-position aligned to middle of tile
    inline float Y() noexcept { return y; }
    //! Get the rotation, 0.0 is upwards/north relative to maze
    inline float Rot() noexcept { return rot; }

private:
    std::unique_ptr<Algorithm> algorithm{nullptr};
    std::unique_ptr<Maze> maze{nullptr};
    float x{0.0};
    float y{0.0};
    float rot{0.0};
};
} // namespace Core
