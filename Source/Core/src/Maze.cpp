#include "Maze.h"

Maze::Maze(int width, int height)
    : tiles{std::vector<MazeTile>(width * height)}, width{width}, height{height}
{
}
