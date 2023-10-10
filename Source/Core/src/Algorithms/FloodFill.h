#pragma once

#include <stack>

#include "Core/Algorithm.h"

namespace Core::Algorithms
{

/*! \brief Base Flood fill algorithm
 *
 *  This version is entirely unweighted. This means it does not account for swinging and movement,
 * only distance
 */
class FloodFill : public Algorithm
{
public:
    using Value = uint16_t;

    struct Coord
    {
        int x;
        int y;
    };

    FloodFill(int width, int height);

    std::optional<Direction> Step(Maze *maze, int x, int y, Direction direction);

    //! Run flood fill algorithm on current maze
    void Flood(Maze *maze, bool to_start);

    //! Debug text for tiles
    std::optional<std::string> GetText(Maze *maze, int x, int y);

private:
    std::vector<Value> tiles;
    int width;
    int height;

    inline Value &GetValue(int x, int y) { return tiles[(width * y) + x]; }
};

} // namespace Core::Algorithms
