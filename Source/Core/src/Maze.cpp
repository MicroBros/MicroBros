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
            tile &= ~(MazeTile::Top | MazeTile::Left | MazeTile::Right | MazeTile::Bottom);
        }
    }
}

} // namespace Core
