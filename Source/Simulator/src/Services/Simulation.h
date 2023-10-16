#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Core/Bitflags.h>
#include <Core/Maze.h>
#include <Core/Mouse.h>

#include "../SimulatorMouse.h"
#include "Service.h"

namespace Simulator::Services
{

/*! \brief State and logic related to Simulation in Simulator
 */
class Simulation : public SimulatorMouse, public Service
{
public:
    Simulation(Application *application);

    void Tick();

    //! Open maze file dialog
    void OpenMaze();
    //! Open maze of path
    void OpenMaze(std::string path);

    /* Mouse */
    inline bool IsSimulation() { return true; };
    bool IsMoving();
    inline bool IsRunning() { return running; };
    inline void SetRunning(bool running) { this->running = running; };
    void Reset();
    void Step();
    Core::Mouse *GetMouse();
    std::vector<std::string> &GetAlgorithms();
    size_t GetAlgorithm();
    void SetAlgorithm(size_t i);

    inline Core::Maze *GetMaze() { return maze.get(); };

    /* Just keep these public for simplicity */
    //! Speed in steps per second
    float speed{3.0f};
    //! X-value before last step
    float last_x;
    //! Y-value before last step
    float last_y;
    //! Rotation before last step
    float last_rot;
    //! Last step in us from application start
    uint64_t last_step{0};

private:
    bool running{false};
    Application *application{nullptr};
    std::unique_ptr<Core::Maze> maze{nullptr};
    std::unique_ptr<Core::Mouse> mouse{nullptr};
    std::vector<std::string> algorithms;
    size_t algorithm{0};
    size_t next_algorithm{0};
    int width, height;

    //! Trace using the Simulation in the Direction, return the hit MazeTile from the Mouse Maze
    Core::MazeTile &TraceTile(Core::Direction direction, int x, int y);
    //! Trace the 3 direction the MicroMouse can see and add it to the Mouse Maze
    void TraceWalls(Core::Direction front_direction, int x, int y);
};

} // namespace Simulator::Services
