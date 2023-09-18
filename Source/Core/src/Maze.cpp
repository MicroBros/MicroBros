#include "Core/Maze.h"

namespace Core
{

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
    switch (direction)
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
    }
}

} // namespace Core
