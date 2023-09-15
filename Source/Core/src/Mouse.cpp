#include "Mouse.h"

Mouse::Mouse(int width, int height, bool initialize_tiles)
{
    maze = std::make_unique<Maze>(width, height);

    // Default initialise the maze values for 16x16 mazes
    if (initialize_tiles && width == 16 && height == 16)
    {
        // Set the start tile
        maze->GetTile(0, 0) |= MazeTile::Start;

        // Add the goal tiles
        maze->GetTile(7, 7) |= MazeTile::Goal;
        maze->GetTile(7, 8) |= MazeTile::Goal;
        maze->GetTile(8, 7) |= MazeTile::Goal;
        maze->GetTile(8, 8) |= MazeTile::Goal;
    }
}

Mouse::Mouse(std::unique_ptr<Maze> maze) : maze{std::move(maze)} {}
