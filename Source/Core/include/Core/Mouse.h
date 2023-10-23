#pragma once

#include <cmath>
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
    //! \brief Set the Algorithm the Mouse will use
    //!
    //! \returns Returns true if the algorithm was set
    bool SetAlgorithm(const std::string value);
    //! \brief Set the Algorithm to the one at \p index in AlgorithmRegistry
    //!
    //! \returns Returns true if the algorithm was set
    bool SetAlgorithm(size_t index);
    //! Get the Algorithm for the Mouse
    inline Algorithm *GetAlgorithm() noexcept { return algorithm.get(); }
    //! Get the Maze for the Mouse
    inline Maze *GetMaze() noexcept { return maze.get(); }
    //! Get the X-position aligned to middle of tile
    inline float X() noexcept { return x; }
    //! Get the Y-position aligned to middle of tile
    inline float Y() noexcept { return y; }
    //! Get the rotation, 0.0 is upwards/north relative to maze, degrees
    inline float Rot() noexcept { return rot; }
    //! Get the closest direction based on rotation
    inline Direction GetDirection() noexcept { return Direction::FromRot(Rot()); }
    //! Temporarily function to set position, used by Simulator
    inline void SetPosition(float x, float y, float rot) noexcept
    {
        this->x = x;
        this->y = y;
        this->rot = rot;
    }
    //! Get if the mouse should be returning to start
    inline bool &ReturnStart() noexcept { return return_start; }
    //! Give the index to get current algorithm in AlgorithmRegistry, returns -1 if uninitialized
    int16_t GetAlgorithmIndex() noexcept { return current_algorithm_index; }

protected:
    std::unique_ptr<Algorithm> algorithm{nullptr};
    std::unique_ptr<Maze> maze{nullptr};
    float x{0.0};
    float y{0.0};
    float rot{0.0};
    bool return_start{false};
    int16_t current_algorithm_index{-1};
};
} // namespace Core
