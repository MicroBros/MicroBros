#include <fstream>

#include <fmt/format.h>
#include <nfd.h>

#include "../Application.h"
#include "Simulation.h"

using namespace Core;

namespace Simulator::Services
{

Simulation::Simulation(Application *application) : application{application}
{
    auto registry{AlgorithmRegistry::GetRegistry()};
    for (auto const &[name, alg] : registry)
    {
        algorithms.push_back(name);
    }
}

void Simulation::Tick()
{
    // If running, auto-step when done moving
    if (IsRunning() && !IsMoving())
        Step();
}

void Simulation::OpenMaze()
{
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = {{"Maze files", "txt"}};
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);

    if (result == NFD_OKAY)
    {
        OpenMaze(std::string(outPath));
        NFD_FreePath(outPath);
    }
    else if (result != NFD_CANCEL)
    {
        auto err{fmt::format("{}", NFD_GetError())};
        application->Error(err);
    }
}

void Simulation::OpenMaze(std::string path)
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

    Reset();
}

/* Mouse */
void Simulation::Reset()
{
    algorithm = next_algorithm;

    if (!mouse)
        return;

    if (algorithm >= algorithms.size())
        throw std::runtime_error("Algorithm index out of range");

    // Stop running & reset Simulation
    running = false;
    last_step = 0;

    // Reset the mouse
    mouse->Reset();

    // Set the algorithm for the Mouse
    mouse->SetAlgorithm(
        std::unique_ptr<Algorithm>(AlgorithmRegistry::GetRegistry()[algorithms[algorithm]](
            mouse.get(), mouse->GetMaze()->GetWidth(), mouse->GetMaze()->GetHeight())));

    // Set up the default walls
    // Add the back wall if start is at 0,0
    MazeTile &mouse_tile{mouse->GetMaze()->GetTile(0, 0)};
    if (mouse_tile.Contains(MazeTile::Start))
        mouse_tile |= MazeTile::Down;
}

void Simulation::Step()
{
    // Stop if no mouse
    if (!mouse)
        return;

    // Update the step time
    last_step = SDL_GetTicks();

    last_x = mouse->X();
    last_y = mouse->Y();
    last_rot = mouse->Rot();

    // Get the absolute x, y the mouse is in
    int x{(int)std::round(mouse->X())};
    int y{(int)std::round(mouse->Y())};
    auto &tile{maze->GetTile(x, y)};

    // For now just stop when Goal is found
    bool is_returning{mouse->ReturnStart()};
    if ((!is_returning && tile.Contains(MazeTile::Goal)) ||
        is_returning && tile.Contains(MazeTile::Start))
        return;

    Direction front_direction{mouse->GetDirection()};

    // Trace the walls if at start
    if (tile.Contains(MazeTile::Start))
        TraceWalls(front_direction, x, y);

    // Step the algorithm
    auto move_direction{mouse->GetAlgorithm()->Step(mouse.get(), x, y, front_direction)};
    if (!move_direction.has_value())
        return fmt::println("No move direction returned by Algorithm!");
    Direction direction{move_direction.value()};

    // Check if crashed
    if (tile.Contains(direction.TileSide()))
        throw std::runtime_error(fmt::format("CRASH, direction: {}", direction.ToString()));

    // Update the position
    switch (direction.Value())
    {
    case Direction::Up:
        y++;
        break;
    case Direction::Right:
        x++;
        break;
    case Direction::Down:
        y--;
        break;
    case Direction::Left:
        x--;
        break;
    }

    mouse->SetPosition(x, y, static_cast<Direction::ValueType>(direction.Value()) * 90.0);

    // Do a new trace to update fake sensor results
    TraceWalls(direction, x, y);
}

MazeTile &Simulation::TraceTile(Direction direction, int x, int y)
{
    switch (direction.Value())
    {
    case Direction::Up:
        while (height > y)
        {
            if (maze->GetTile(x, y).Contains(MazeTile::Up))
                return mouse->GetMaze()->GetTile(x, y);
            y++;
        }
        break;
    case Direction::Right:
        while (width > x)
        {
            if (maze->GetTile(x, y).Contains(MazeTile::Right))
                return mouse->GetMaze()->GetTile(x, y);
            x++;
        }
        break;
    case Direction::Down:
        while (y >= 0)
        {
            if (maze->GetTile(x, y).Contains(MazeTile::Down))
                return mouse->GetMaze()->GetTile(x, y);
            y--;
        }
        break;
    case Direction::Left:
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

void Simulation::TraceWalls(Direction front_direction, int x, int y)
{
    // Find the global front, left and right directions of the Mouse
    Direction left_direction{front_direction.TurnLeft()};
    Direction right_direction{front_direction.TurnRight()};

    // Trace the three local sensor directions and add the walls
    TraceTile(front_direction, x, y) |= front_direction.TileSide();
    TraceTile(left_direction, x, y) |= left_direction.TileSide();
    TraceTile(right_direction, x, y) |= right_direction.TileSide();
}

bool Simulation::IsMoving()
{
    uint64_t step_time{(uint64_t)(1.0 / speed * 1000)};
    return last_step > SDL_GetTicks() - step_time;
}

Mouse *Simulation::GetMouse() { return mouse.get(); }

std::vector<std::string> &Simulation::GetAlgorithms() { return algorithms; }
size_t Simulation::GetAlgorithm() { return algorithm; }
void Simulation::SetAlgorithm(size_t i) { next_algorithm = i; }

} // namespace Simulator::Services
