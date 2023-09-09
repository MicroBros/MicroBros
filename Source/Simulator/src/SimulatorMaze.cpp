#include <fstream>
#include <fmt/format.h>
#include <stdexcept>

#include "SimulatorMaze.h"

SimulatorMaze::SimulatorMaze(int width, int height) : width{width},
                                                      height{height},
                                                      tiles{std::vector<SimulatorMazeTile>(width * height)}
{
}

SimulatorMaze::SimulatorMaze(std::string path)
{
    // Open the maze file
    std::ifstream input_maze;
    input_maze.open(path, std::ios::in);
    if (!input_maze)
    {
        throw std::runtime_error(fmt::format("Error opening file at: {}", path));
    }

    // Read the lines
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input_maze, line))
    {
        if (line != "")
            lines.emplace_back(line);
    }

    // Find the size of the maze map
    if (lines.empty())
        throw std::runtime_error("Empty map file!");

    width = {(static_cast<int>(lines[0].length()) - 1) / 4};
    height = ((static_cast<int>(lines.size()) - 1) / 2);

    if (width != height)
        throw std::runtime_error(fmt::format("Map expected to be square! Got {}x{}", width, height));

    // Initialise the tiles
    tiles = std::vector<SimulatorMazeTile>(width * height);

    for (int y{0}; y < height; ++y)
    {
        for (int x{0}; x < width; ++x)
        {
            size_t i{GetIndex(x, y)};
            SimulatorMazeTile tile = (SimulatorMazeTile)0;

#define CHECK_DIRECTION(dir, x, y) \
    if (lines.at(y).at(x) != ' ')  \
        tile = tile | dir;

            // Get the different wall directions
            CHECK_DIRECTION(SimulatorMazeTile::Top, (x * 4) + 2, y * 2)
            CHECK_DIRECTION(SimulatorMazeTile::Bottom, (x * 4) + 2, (y * 2) + 2)
            CHECK_DIRECTION(SimulatorMazeTile::Left, x * 4, (y * 2) + 1)
            CHECK_DIRECTION(SimulatorMazeTile::Right, (x * 4) + 4, (y * 2) + 1)

            // Find the center character of the tile
            char center{lines.at((y * 2) + 1).at((x * 4) + 2)};

            switch (center)
            {
            case 'G':
                tile = tile | SimulatorMazeTile::Goal;
                break;
            case 'S':
                tile = tile | SimulatorMazeTile::Start;
                break;
            default:
                break;
            }

            tiles[i] = tile;
        }
    }
}

size_t SimulatorMaze::GetIndex(int x, int y)
{
    return (width * y + x);
}

const float BORDER_THICKNESS{3};
const ImVec2 BORDER_THICKNESS_VEC{BORDER_THICKNESS, BORDER_THICKNESS};
void SimulatorMaze::Draw()
{
    // Use the shortest side as size and longest maze size to ensure the entire thing is drawn
    ImVec2 window_pos{ImGui::GetWindowPos()};
    ImVec2 content_min{ImGui::GetWindowContentRegionMin()};
    ImVec2 content_max{ImGui::GetWindowContentRegionMax()};
    ImVec2 pos{window_pos + content_min + ImVec2(8.0, 0)};
    ImVec2 size{content_max - content_min};

    float s{std::min(size.x, size.y)};
    int maze_size{std::max(width, height)};

    float per{(s - BORDER_THICKNESS) / maze_size};

    auto draw_list = ImGui::GetWindowDrawList();

    for (int y{0}; y < (height + 1); ++y)
    {
        for (int x{0}; x < (width + 1); ++x)
        {
            if (!(x >= width || y >= height))
            {
                SimulatorMazeTile tile{tiles[GetIndex(x, y)]};

                // Draw the goals
                if (tile & SimulatorMazeTile::Goal)
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC, pos + ImVec2(per * (x + 1), per * (y + 1)), ImColor(100, 20, 20));
                }
                else if (tile & SimulatorMazeTile::Start)
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC, pos + ImVec2(per * (x + 1), per * (y + 1)), ImColor(0, 100, 0));
                }

                // Draw the walls
                if (tile & SimulatorMazeTile::Top)
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x + BORDER_THICKNESS, per * y), pos + ImVec2(per * (x + 1), per * y + BORDER_THICKNESS), ImColor(200, 0, 0));
                }
                if (tile & SimulatorMazeTile::Bottom)
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x + BORDER_THICKNESS, per * (y + 1)), pos + ImVec2(per * (x + 1), per * (y + 1) + BORDER_THICKNESS), ImColor(200, 0, 0));
                }
                if (tile & SimulatorMazeTile::Left)
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x, per * y + BORDER_THICKNESS), pos + ImVec2(per * x + BORDER_THICKNESS, per * (y + 1)), ImColor(200, 0, 0));
                }
                if (tile & SimulatorMazeTile::Right)
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * (x + 1), per * y + BORDER_THICKNESS), pos + ImVec2(per * (x + 1) + BORDER_THICKNESS, per * (y + 1)), ImColor(200, 0, 0));
                }
            }

            // Draw the corners of every tile
            draw_list->AddRect(pos + ImVec2(per * x, per * y), pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC, ImColor(255, 255, 255));
        }
    }
}
