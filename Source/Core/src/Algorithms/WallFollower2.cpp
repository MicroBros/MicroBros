#include <array>

#include "Core/Algorithm.h"

namespace Core::Algorithms
{

using Visits = uint16_t;

/*! \brief Improved wall following algorithm
 *
 *  This algorithm is based on following the right wall in the Maze but also keeping track of times
 * a MazeTile is visited and the least visited tile in the order right, front, left, back order will
 * be taken
 */
class WallFollower2 : public Algorithm
{
public:
    WallFollower2(int width, int height)
        : tiles{std::vector<Visits>(width * height)}, width{width}, height{height}
    {
    }

    std::optional<Direction> Step(Maze *maze, int x, int y, Direction direction)
    {
        // Add a visit to current tile
        GetVisits(x, y) += 1;

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
        auto min_visits{std::numeric_limits<Visits>::max()};
        std::optional<Direction> go_direction{std::nullopt};

        for (auto &try_direction : direction_order)
        {
            if (!tile.Contains(try_direction.TileSide()))
            {
                int test_x{x};
                int test_y{y};

                switch (try_direction.Value())
                {
                case Direction::Up:
                    test_y++;
                    break;
                case Direction::Right:
                    test_x++;
                    break;
                case Direction::Down:
                    test_y--;
                    break;
                case Direction::Left:
                    test_x--;
                    break;
                default:
                    break;
                }

                // Go this direction if less visited
                auto direction_visits{GetVisits(test_x, test_y)};
                if (min_visits > direction_visits)
                {
                    min_visits = direction_visits;
                    go_direction = try_direction;
                }
            }
        }

        // Did not find a place to turn to, should never happen unless the Mouse is in a 1x1
        // space
        return go_direction;
    }

    std::optional<std::string> GetText(Maze *maze, int x, int y)
    {
        return std::to_string(GetVisits(x, y));
    }

private:
    std::vector<Visits> tiles;
    int width;
    int height;

    inline Visits &GetVisits(int x, int y) { return tiles[(width * y) + x]; }
};

REGISTER_ALGORITHM(WallFollower2)

} // namespace Core::Algorithms
