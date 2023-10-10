#include "Core/Maze.h"

namespace Core
{

/* Direction */
Direction Direction::FromRot(float rot)
{
    // Divide the rotation and then round it to make it match the indexes of the Direction enum
    return Direction(static_cast<Direction::ValueEnum>(static_cast<int>(std::round(rot / 90.0)) %
                                                       Direction::Values));
}

std::string_view Direction::ToString()
{
    switch (direction)
    {
    case Core::Direction::Up:
        return "Up";
    case Core::Direction::Right:
        return "Right";
    case Core::Direction::Down:
        return "Down";
    case Core::Direction::Left:
        return "Left";
    default:
        return "UNKNOWN";
    }
}

/* Maze */
Maze::Maze(int width, int height)
    : tiles{std::vector<MazeTile>(width * height)}, width{width}, height{height}
{
}

void Maze::ResetWalls()
{
    for (int y{0}; y < height; ++y)
    {
        for (int x{0}; x < width; ++x)
        {
            MazeTile &tile{GetTile(x, y)};
            tile &= ~(MazeTile::Up | MazeTile::Left | MazeTile::Right | MazeTile::Down);
        }
    }
}

bool Maze::HasWall(int x, int y, Direction direction)
{
    // Check the tile and adjecent if one of them has had a wall registered
    switch (direction.Value())
    {
    case Direction::Up:
        return y >= (height - 1) ? GetTile(x, y).Contains(MazeTile::Up)
                                 : GetTile(x, y).Contains(MazeTile::Up) ||
                                       GetTile(x, y + 1).Contains(MazeTile::Down);
    case Direction::Right:
        return x >= (width - 1) ? GetTile(x, y).Contains(MazeTile::Right)
                                : GetTile(x, y).Contains(MazeTile::Right) ||
                                      GetTile(x + 1, y).Contains(MazeTile::Left);
    case Direction::Down:
        return y > 0 ? GetTile(x, y).Contains(MazeTile::Down) ||
                           GetTile(x, y - 1).Contains(MazeTile::Up)
                     : GetTile(x, y).Contains(MazeTile::Down);
    case Direction::Left:
        return x > 0 ? GetTile(x, y).Contains(MazeTile::Left) ||
                           GetTile(x - 1, y).Contains(MazeTile::Right)
                     : GetTile(x, y).Contains(MazeTile::Left);
    default:
        return false;
    }
}

MazeTile &Maze::GetTileAdjacent(int x, int y, Direction direction)
{
    int next_x{x};
    int next_y{y};

    if (direction.Value() == Direction::Up)
        next_y++;
    else if (direction.Value() == Direction::Right)
        next_x++;
    else if (direction.Value() == Direction::Down)
        next_y--;
    else if (direction.Value() == Direction::Left)
        next_x--;

    return GetTile(next_x, next_y);
}

} // namespace Core
