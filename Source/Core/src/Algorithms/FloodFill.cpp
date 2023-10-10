#include "FloodFill.h"

#include <array>

#include <Core/Log.h>

namespace Core::Algorithms
{

FloodFill::FloodFill(int width, int height) : width{width}, height{height}, tiles(width * height, 0)
{
}

std::optional<Direction> FloodFill::Step(Maze *maze, int x, int y, Direction direction)
{
    Flood(maze, false);

    // Get the global back, left and right directions
    Direction left_direction{direction.TurnLeft()};
    Direction right_direction{direction.TurnRight()};
    Direction back_direction{direction.TurnRight(2)};

    // The four directions ordered with front, right, left, back
    std::array<Direction, 4> direction_order{direction, right_direction, left_direction,
                                             back_direction};

    // Get the current tile
    auto &tile{maze->GetTile(x, y)};

    // Try the four directions in order and see if one can move there
    auto min_value{std::numeric_limits<Value>::max()};
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

            // Go this direction if less value
            auto direction_value{GetValue(test_x, test_y)};
            if (min_value > direction_value)
            {
                min_value = direction_value;
                go_direction = try_direction;
            }
        }
    }

    return go_direction;
}

void FloodFill::Flood(Maze *maze, bool to_start)
{
    std::stack<Coord> stack;

    // Set the zeros and add to stack the goals
    for (int y{0}; y < height; ++y)
    {
        for (int x{0}; x < width; ++x)
        {
            MazeTile &tile{maze->GetTile(x, y)};
            if ((to_start && tile.Contains(MazeTile::Start)) ||
                (!to_start && tile.Contains(MazeTile::Goal)))
            {
                stack.push(Coord{x, y});
                GetValue(x, y) = 0;
            }
            else
            {
                GetValue(x, y) = std::numeric_limits<Value>::max();
            }
        }
    }

    while (!stack.empty())
    {
        Coord coord{stack.top()};
        stack.pop();

        Value &value{GetValue(coord.x, coord.y)};
        Value next_value = value + 1;

        // Preprocessor macro for "flooding" a direction
#define FLOOD_DIRECTION(DIR, X, Y)                                                                 \
    {                                                                                              \
        int x{coord.x + X};                                                                        \
        int y{coord.y + Y};                                                                        \
        if (maze->WithinBounds(x, y) && GetValue(x, y) > next_value &&                             \
            !maze->HasWall(coord.x, coord.y, Direction::DIR))                                      \
        {                                                                                          \
            stack.push(Coord{x, y});                                                               \
            GetValue(x, y) = next_value;                                                           \
        }                                                                                          \
    }

        // Run flooding
        FLOOD_DIRECTION(Left, -1, 0)
        FLOOD_DIRECTION(Right, 1, 0)
        FLOOD_DIRECTION(Up, 0, 1)
        FLOOD_DIRECTION(Down, 0, -1)
    }
}

std::optional<std::string> FloodFill::GetText(Maze *maze, int x, int y)
{
    return std::to_string(GetValue(x, y));
}

REGISTER_ALGORITHM(FloodFill)

} // namespace Core::Algorithms
