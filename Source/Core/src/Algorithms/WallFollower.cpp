#include <array>

#include "Core/Algorithm.h"

namespace Core::Algorithms
{

/*! \brief Basic wall following algorithm
 *
 *  This algorithm is based on following the right wall in the Maze
 */
class WallFollower : public Algorithm
{
public:
    WallFollower(int width, int height) {}

    std::optional<Direction> Step(Maze *maze, int x, int y, Direction direction)
    {
        // Get the global back, left and right directions
        Direction left_direction{direction.TurnLeft()};
        Direction right_direction{direction.TurnRight()};
        Direction back_direction{direction.TurnRight(2)};

        // The four directions ordered with right, front, left, back
        std::array<Direction, 4> direction_order{right_direction, direction, left_direction,
                                                 back_direction};

        // Get the current tile
        auto &tile{maze->GetTile(x, y)};

        // Try the four directions in order and see if one can move there
        for (auto &try_direction : direction_order)
        {
            if (!tile.Contains(try_direction.TileSide()))
            {
                return try_direction;
            }
        }

        // Did not find a place to turn to, should never happen unless the Mouse is in a 1x1
        // space
        return std::nullopt;
    }
};

REGISTER_ALGORITHM(WallFollower)

} // namespace Core::Algorithms
