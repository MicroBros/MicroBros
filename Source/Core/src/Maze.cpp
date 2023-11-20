#include "Core/Maze.h"
#include "Core/Log.h"

namespace Core
{

/* Direction */
Direction Direction::FromRot(float rot)
{
    // Divide the rotation and then round it to make it match the indexes of the Direction enum
    return Direction(static_cast<Direction::ValueEnum>(static_cast<int>(std::round(rot / 90.0)) %
                                                       Direction::Values));
}

int Direction::Degrees()
{
    switch (direction)
    {
    case Core::Direction::Up:
        return 0;
    case Core::Direction::Right:
        return 90;
    case Core::Direction::Down:
        return 180;
    case Core::Direction::Left:
        return 270;
    default:
        return 0;
    }
}

std::string_view Direction::ToString()
{
    switch (direction)
    {
    case Core::Direction::Up:
        return "Up/Forward";
    case Core::Direction::Right:
        return "Right";
    case Core::Direction::Down:
        return "Down/Backward";
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

Maze::Maze(int width, int height, std::span<MazeTile::ValueType> data)
    : tiles{std::vector<MazeTile>(width * height)}, width{width}, height{height}
{
    if (data.size() != (width * height))
    {
#ifdef FIRMWARE
        LOG_ERROR("Invalid maze data size: {}, expected: {}", data.size(), width * height);
        return;
#else
        throw std::runtime_error(
            fmt::format("Invalid maze data size: {}, expected: {}", data.size(), width * height));
#endif
    }
    else
    {
        // Copy data over
        tiles.assign(data.begin(), data.end());
    }
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

MazeTile &Maze::GetTileAdjacent(int x, int y, Direction direction, int offset)
{
    int next_x{x};
    int next_y{y};

    switch (direction.Value())
    {
    case Direction::Up:
        next_y += offset;
        break;
    case Direction::Right:
        next_x += offset;
        break;
    case Direction::Down:
        next_y -= offset;
        break;
    case Direction::Left:
        next_x -= offset;
        break;
    }

    return GetTile(next_x, next_y);
}

} // namespace Core
