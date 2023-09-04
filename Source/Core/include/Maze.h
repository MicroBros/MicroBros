#pragma once

#include <vector>

class MazeTile
{
};

class Maze
{
public:
    Maze(int width, int height);

private:
    std::vector<MazeTile> tiles;
};
