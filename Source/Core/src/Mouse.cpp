#include "Core/Mouse.h"

namespace Core
{

Mouse::Mouse(int width, int height, bool initialize_tiles)
{
    maze = std::make_unique<Maze>(width, height);

    // Default initialise the maze values for 16x16 mazes
    if (initialize_tiles && width == 16 && height == 16)
    {
        // Set the start tile
        maze->GetTile(0, 0) |= MazeTile::Start | MazeTile::Down;

        // Add the goal tiles
        maze->GetTile(7, 7) |= MazeTile::Goal;
        maze->GetTile(7, 8) |= MazeTile::Goal;
        maze->GetTile(8, 7) |= MazeTile::Goal;
        maze->GetTile(8, 8) |= MazeTile::Goal;
    }
}

Mouse::Mouse(std::unique_ptr<Maze> maze) : maze{std::move(maze)} {}

bool Mouse::SetAlgorithm(const std::string value)
{
    auto registry{AlgorithmRegistry::GetRegistry()};

    // Calculate index in registry map
    size_t i{0};
    AlgorithmRegistry::Registry::iterator it;
    for (it = registry.begin(); it != registry.end(); ++it)
    {
        if (it->first == value)
            break;
        else if (it == registry.end())
            return false;
        i++;
    }

    current_algorithm_index = i;

    // Construct the algorithm
    algorithm =
        std::unique_ptr<Algorithm>(it->second(this, GetMaze()->GetWidth(), GetMaze()->GetHeight()));

    return true;
}

bool Mouse::SetAlgorithm(size_t index)
{
    auto registry{AlgorithmRegistry::GetRegistry()};

    AlgorithmRegistry::Registry::iterator it;
    for (size_t i{0}; i < index; ++i)
    {
        if (it == registry.end())
            return false;
        it++;
    }

    current_algorithm_index = index;

    // Construct the algorithm
    algorithm =
        std::unique_ptr<Algorithm>(it->second(this, GetMaze()->GetWidth(), GetMaze()->GetHeight()));
}

void Mouse::Reset()
{
    x = 0.0;
    y = 0.0;
    rot = 0.0;
    maze->ResetWalls();
}

} // namespace Core
