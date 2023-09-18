#include <cmath>
#include <fstream>
#include <stdexcept>

#include <fmt/format.h>

#include <Core/Algorithm.h>

#include "SimulatorMaze.h"

using namespace Core;

namespace Simulator
{

//! Enum with a wall is present
enum class WallPresent : uint8_t
{
    No = 0,
    Simulator,
    Mouse,
};

//! Check both the Mouse Maze and SimulatorMaze if a wall is present (WalLPresent)
WallPresent CheckWalls(Maze *mouse_maze, Maze *simulator_maze, int x, int y, Direction direction)
{
    if (mouse_maze->HasWall(x, y, direction))
    {
        return WallPresent::Mouse;
    }
    else if (simulator_maze->HasWall(x, y, direction))
    {
        return WallPresent::Simulator;
    }

    return WallPresent::No;
}

inline ImColor WallColor(WallPresent present) noexcept
{
    return present == WallPresent::Mouse ? ImColor(200, 0, 0) : ImColor(80, 80, 80);
}

SimulatorMaze::SimulatorMaze(int width, int height)
    : width{width}, height{height}, maze{std::make_unique<Maze>(width, height)},
      mouse{std::make_unique<Mouse>(width, height)}
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
        throw std::runtime_error(
            fmt::format("Map expected to be square! Got {}x{}", width, height));

    // Initialise the maze
    maze = std::make_unique<Maze>(width, height);

    for (int y{0}; y < height; ++y)
    {
        for (int x{0}; x < width; ++x)
        {
            // 0, 0 is at bottom left
            MazeTile &tile{maze->GetTile(x, height - y - 1)};

#define CHECK_DIRECTION(dir, x, y)                                                                 \
    if (lines.at(y).at(x) != ' ')                                                                  \
        tile = tile | dir;

            // Get the different wall directions
            CHECK_DIRECTION(MazeTile::Up, (x * 4) + 2, y * 2)
            CHECK_DIRECTION(MazeTile::Down, (x * 4) + 2, (y * 2) + 2)
            CHECK_DIRECTION(MazeTile::Left, x * 4, (y * 2) + 1)
            CHECK_DIRECTION(MazeTile::Right, (x * 4) + 4, (y * 2) + 1)

            // Find the center character of the tile
            char center{lines.at((y * 2) + 1).at((x * 4) + 2)};

            switch (center)
            {
            case 'G':
                tile |= MazeTile::Goal;
                break;
            case 'S':
                tile |= MazeTile::Start;
                break;
            default:
                break;
            }
        }
    }

    // Create a maze for the mouse, but do not include the walls
    auto mouse_maze = std::make_unique<Maze>(width, height);
    for (int y{0}; y < height; ++y)
    {
        for (int x{0}; x < width; ++x)
        {
            MazeTile &tile{maze->GetTile(x, y)};
            MazeTile &mouse_tile{mouse_maze->GetTile(x, y)};
            if (tile.Contains(MazeTile::Goal))
                mouse_tile |= MazeTile::Goal;
            if (tile.Contains(MazeTile::Start))
                mouse_tile |= MazeTile::Start;
        }
    }

    // Initialise the mouse
    mouse = std::make_unique<Mouse>(std::move(mouse_maze));
}

// Simulation
void SimulatorMaze::Step()
{
    // Get the absolute x, y the mouse is in
    int x{(int)std::round(mouse->X())};
    int y{(int)std::round(mouse->Y())};
    auto &tile{maze->GetTile(x, y)};

    // Find the global front, left and right directions of the Mouse
    Core::Direction front_direction{mouse->GetDirection()};
    Core::Direction left_direction{front_direction.TurnLeft()};
    Core::Direction right_direction{front_direction.TurnRight()};

    // Trace the three local sensor directions and add the walls
    TraceTile(front_direction, x, y) |= front_direction.TileSide();
    TraceTile(left_direction, x, y) |= left_direction.TileSide();
    TraceTile(right_direction, x, y) |= right_direction.TileSide();

    // Step the algorithm
    auto move_direction{mouse->GetAlgorithm()->Step(maze.get(), x, y, front_direction)};
    if (!move_direction.has_value())
        return fmt::println("No move direction returned by Algorithm!");
    Core::Direction direction{move_direction.value()};

    // Check if crashed
    if (tile.Contains(direction.TileSide()))
        throw std::runtime_error(fmt::format("CRASH, direction: {}", direction.ToString()));

    // Update the position
    switch (direction.Value())
    {
    case Core::Direction::Up:
        y++;
        break;
    case Core::Direction::Right:
        x++;
        break;
    case Core::Direction::Down:
        y--;
        break;
    case Core::Direction::Left:
        x--;
        break;
    }

    mouse->SetPosition(x, y, static_cast<Core::Direction::ValueType>(direction.Value()) * 90.0);
}

Core::MazeTile &SimulatorMaze::TraceTile(Core::Direction direction, int x, int y)
{
    switch (direction.Value())
    {
    case Core::Direction::Up:
        while (height > y)
        {
            if (maze->GetTile(x, y).Contains(MazeTile::Up))
                return mouse->GetMaze()->GetTile(x, y);
            y++;
        }
        break;
    case Core::Direction::Right:
        while (width > x)
        {
            if (maze->GetTile(x, y).Contains(MazeTile::Right))
                return mouse->GetMaze()->GetTile(x, y);
            x++;
        }
        break;
    case Core::Direction::Down:
        while (y >= 0)
        {
            if (maze->GetTile(x, y).Contains(MazeTile::Down))
                return mouse->GetMaze()->GetTile(x, y);
            y--;
        }
        break;
    case Core::Direction::Left:
        while (x >= 0)
        {
            if (maze->GetTile(x, y).Contains(MazeTile::Left))
                return mouse->GetMaze()->GetTile(x, y);
            x--;
        }
        break;
    default:
        throw std::runtime_error(
            fmt::format("Invalid TraceTile direction: {}", direction.ToString()));
    }

    throw std::runtime_error(fmt::format("Maze lacks other wall, ran out at {},{}", x, y));
}

void SimulatorMaze::Reset()
{
    running = false;
    mouse->Reset();

    // Check if a algorithm has been set
    if (!algorithm.has_value())
        throw std::runtime_error("No algorithm has been selected");

    // Find the algorithm constructor
    auto registry{Core::AlgorithmRegistry::GetRegistry()};
    auto algorithm_constructor{registry.find(algorithm.value())};
    if (algorithm_constructor == registry.end())
        throw std::runtime_error(
            fmt::format("Algorithm \"{}\" was not found in AlgorithmRegistry!", algorithm.value()));

    // Set the algorithm
    mouse->SetAlgorithm(std::unique_ptr<Algorithm>(algorithm_constructor->second(width, height)));

    // Set up the default walls
    // Add the back wall if start is at 0,0
    MazeTile &mouse_tile{mouse->GetMaze()->GetTile(0, 0)};
    if (mouse_tile.Contains(MazeTile::Start))
        mouse_tile |= MazeTile::Down;
}

// Drawing
const float BORDER_THICKNESS{3};
const ImVec2 BORDER_THICKNESS_VEC{BORDER_THICKNESS, BORDER_THICKNESS};
void SimulatorMaze::Draw(Utils::Texture *mouse_sprite)
{
    // Use the shortest side as size and longest maze size to ensure the entire thing is
    // drawn
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
            // It is rendering top-left as 0,0. but the maze uses
            // bottom-left as 0,0. Correct for it by inverting the y
            int maze_y{height - y - 1};

            // Ignore the extra height/width used for corners
            if (!(x >= width || y >= height))
            {
                MazeTile &tile{maze->GetTile(x, maze_y)};

                // Draw the goals
                if (tile.Contains(MazeTile::Goal))
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC,
                                             pos + ImVec2(per * (x + 1), per * (y + 1)),
                                             ImColor(0, 100, 0));
                }
                else if (tile.Contains(MazeTile::Start))
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC,
                                             pos + ImVec2(per * (x + 1), per * (y + 1)),
                                             ImColor(100, 20, 20));
                }

                WallPresent wall_right{
                    CheckWalls(mouse->GetMaze(), maze.get(), x, maze_y, Direction::Right)};
                if (wall_right != WallPresent::No)
                {
                    draw_list->AddRectFilled(
                        pos + ImVec2(per * (x + 1), per * y + BORDER_THICKNESS),
                        pos + ImVec2(per * (x + 1) + BORDER_THICKNESS, per * (y + 1)),
                        WallColor(wall_right));
                }

                WallPresent wall_up{
                    CheckWalls(mouse->GetMaze(), maze.get(), x, maze_y, Direction::Up)};
                if (wall_up != WallPresent::No)
                {
                    draw_list->AddRectFilled(pos + ImVec2(per * x + BORDER_THICKNESS, per * y),
                                             pos +
                                                 ImVec2(per * (x + 1), per * y + BORDER_THICKNESS),
                                             WallColor(wall_up));
                }

                if (x == 0)
                {
                    WallPresent wall_left{
                        CheckWalls(mouse->GetMaze(), maze.get(), x, maze_y, Direction::Left)};

                    if (wall_left != WallPresent::No)
                    {
                        draw_list->AddRectFilled(
                            pos + ImVec2(per * x, per * y + BORDER_THICKNESS),
                            pos + ImVec2(per * x + BORDER_THICKNESS, per * (y + 1)),
                            WallColor(wall_left));
                    }
                }

                if (maze_y == 0)
                {
                    WallPresent wall_down{
                        CheckWalls(mouse->GetMaze(), maze.get(), x, maze_y, Direction::Down)};

                    if (wall_down != WallPresent::No)
                    {
                        draw_list->AddRectFilled(
                            pos + ImVec2(per * x + BORDER_THICKNESS, per * (y + 1)),
                            pos + ImVec2(per * (x + 1), per * (y + 1) + BORDER_THICKNESS),
                            WallColor(wall_down));
                    }
                }
            }

            // Draw the corners of every tile
            draw_list->AddRect(pos + ImVec2(per * x, per * y),
                               pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC,
                               ImColor(255, 255, 255));
        }
    }

    // Draw mouse
    float x{mouse->X()};
    float y{height - 1 - mouse->Y()};

    /*draw_list->AddImage((ImTextureID)mouse_sprite->Tex(),
                        pos + ImVec2(per * x, per * y) + ImVec2(3, 3),
                        pos + ImVec2(per * (x + 1), per * (y + 1)) - ImVec2(3, 3));*/
    mouse_sprite->DrawRotated(
        pos + ImVec2(per * x, per * y) +
            ImVec2((per + BORDER_THICKNESS) / 2, (per + BORDER_THICKNESS) / 2),
        ImVec2(40, 40), mouse->Rot());
}

} // namespace Simulator
