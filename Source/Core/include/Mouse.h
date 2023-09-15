#pragma once

#include <memory>

#include "Maze.h"

class Mouse
{
public:
    Mouse(int width = 16, int height = 16, bool initialize_tiles = true);
    Mouse(std::unique_ptr<Maze> maze);

    inline Maze *GetMaze() { return maze.get(); }

private:
    std::unique_ptr<Maze> maze{nullptr};
};
