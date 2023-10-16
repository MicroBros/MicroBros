#include <cmath>

#include <imgui.h>

#include "../Application.h"
#include "../Services/Simulation.h"
#include "Window.h"

using namespace Core;

namespace Simulator::Windows
{

//! Enum defining if a wall is present checking the Simulator and Mouse mazes
enum class WallPresent : uint8_t
{
    No = 0,
    Simulator,
    Mouse,
};

//! Maze visualization Window for SimulatorMouse
class Maze : public Window
{
public:
    Maze(Application *application) : application{application} { SetOpen(true); }

    WINDOW(Maze);

    void Draw()
    {
        auto simulator_mouse{application->GetSimulatorMouse()};
        if (!simulator_mouse || !simulator_mouse->GetMouse())
            return;

        ImGui::SetNextWindowSize(ImVec2(800.0f, 800.0f));
        ImGui::Begin("Maze", NULL, ImGuiWindowFlags_NoResize);
        DrawMaze(simulator_mouse);
        ImGui::End();
    }

private:
    Application *application{nullptr};

    //! Check both the Mouse Maze and SimulatorMaze if a wall is present (WallPresent)
    WallPresent CheckWalls(SimulatorMouse *simulator_mouse, int x, int y, Direction direction)
    {
        if (simulator_mouse->GetMouse()->GetMaze()->HasWall(x, y, direction))
        {
            return WallPresent::Mouse;
        }
        else if (simulator_mouse->IsSimulation() &&
                 dynamic_cast<Services::Simulation *>(simulator_mouse)
                     ->GetMaze()
                     ->HasWall(x, y, direction))
        {
            return WallPresent::Simulator;
        }

        return WallPresent::No;
    }

    //! Get the color of a wall depending on if present in Maze file or Mouse
    inline ImColor WallColor(WallPresent present) noexcept
    {
        return present == WallPresent::Mouse ? ImColor(200, 0, 0) : ImColor(80, 80, 80);
    }

    const float BORDER_THICKNESS{3};
    const ImVec2 BORDER_THICKNESS_VEC{BORDER_THICKNESS, BORDER_THICKNESS};
    void DrawMaze(SimulatorMouse *simulator_mouse)
    {
        // Setup mouse variables
        auto mouse{simulator_mouse->GetMouse()};
        auto maze{mouse->GetMaze()};
        auto algorithm{mouse->GetAlgorithm()};

        // Use the shortest side as size and longest maze
        // size to ensure the entire thing is drawn
        ImVec2 window_pos{ImGui::GetWindowPos()};
        ImVec2 content_min{ImGui::GetWindowContentRegionMin()};
        ImVec2 content_max{ImGui::GetWindowContentRegionMax()};
        ImVec2 pos{window_pos + content_min + ImVec2(8.0, 0)};
        ImVec2 size{content_max - content_min};
        float s{std::min(size.x, size.y)};

        // Get maze size
        int width{maze->GetWidth()};
        int height{maze->GetHeight()};
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
                    MazeTile &tile{mouse->GetMaze()->GetTile(x, maze_y)};

                    // Draw the goals
                    if (tile.Contains(MazeTile::Goal))
                    {
                        draw_list->AddRectFilled(
                            pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC,
                            pos + ImVec2(per * (x + 1), per * (y + 1)), ImColor(0, 100, 0));
                    }
                    else if (tile.Contains(MazeTile::Start))
                    {
                        draw_list->AddRectFilled(
                            pos + ImVec2(per * x, per * y) + BORDER_THICKNESS_VEC,
                            pos + ImVec2(per * (x + 1), per * (y + 1)), ImColor(100, 20, 20));
                    }

                    WallPresent wall_right{
                        CheckWalls(simulator_mouse, x, maze_y, Direction::Right)};
                    if (wall_right != WallPresent::No)
                    {
                        draw_list->AddRectFilled(
                            pos + ImVec2(per * (x + 1), per * y + BORDER_THICKNESS),
                            pos + ImVec2(per * (x + 1) + BORDER_THICKNESS, per * (y + 1)),
                            WallColor(wall_right));
                    }

                    WallPresent wall_up{CheckWalls(simulator_mouse, x, maze_y, Direction::Up)};
                    if (wall_up != WallPresent::No)
                    {
                        draw_list->AddRectFilled(
                            pos + ImVec2(per * x + BORDER_THICKNESS, per * y),
                            pos + ImVec2(per * (x + 1), per * y + BORDER_THICKNESS),
                            WallColor(wall_up));
                    }

                    if (x == 0)
                    {
                        WallPresent wall_left{
                            CheckWalls(simulator_mouse, x, maze_y, Direction::Left)};

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
                            CheckWalls(simulator_mouse, x, maze_y, Direction::Down)};

                        if (wall_down != WallPresent::No)
                        {
                            draw_list->AddRectFilled(
                                pos + ImVec2(per * x + BORDER_THICKNESS, per * (y + 1)),
                                pos + ImVec2(per * (x + 1), per * (y + 1) + BORDER_THICKNESS),
                                WallColor(wall_down));
                        }
                    }

                    if (algorithm)
                    {
                        // Check if the algorithm has text for the tile
                        auto text{mouse->GetAlgorithm()->GetText(mouse, x, maze_y)};
                        if (text.has_value())
                        {
                            auto text_cstr{text.value().c_str()};
                            auto text_size = ImGui::CalcTextSize(text_cstr);

                            draw_list->AddText(pos + ImVec2(per * x, per * y) +
                                                   ImVec2((per + BORDER_THICKNESS) / 2,
                                                          (per + BORDER_THICKNESS) / 2) -
                                                   (text_size / 2),
                                               ImColor(255, 255, 255), text_cstr);
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
        auto mouse_sprite{application->GetMainWindow()->GetMouseSprite()};

        if (simulator_mouse->IsSimulation() && simulator_mouse->IsMoving())
        {
            auto simulation{dynamic_cast<Services::Simulation *>(simulator_mouse)};
            // Calculate the estimated step time in ms
            float step_time{1.0f / simulation->speed * 1000.0f};
            // Calculate the progress between the start of step and end of step (0.0 - 1.0)
            float progress = ((SDL_GetTicks() - simulation->last_step) /
                              ((simulation->last_step + step_time) - simulation->last_step));

            // Get the top-left, corrected maze y
            float maze_last_y{height - 1.0f - simulation->last_y};

            // Diff the coordinates to find what one needs to move within the step
            float diff_x{x - simulation->last_x};
            float diff_y{y - maze_last_y};

            // Interpolate
            float interp_x{simulation->last_x + (diff_x * progress)};
            float interp_y{maze_last_y + (diff_y * progress)};
            // Based on formula taken from https://stackoverflow.com/a/14498790
            float interp_rot =
                simulation->last_rot +
                ((std::fmod((std::fmod((mouse->Rot() - simulation->last_rot), 360.0f) + 540.0f),
                            360.0f) -
                  180.0f) *
                 std::min(progress * 2, 1.0f));

            // Draw mouse sprite
            mouse_sprite->DrawRotated(
                pos + ImVec2(per * interp_x, per * interp_y) +
                    ImVec2((per + BORDER_THICKNESS) / 2, (per + BORDER_THICKNESS) / 2),
                ImVec2(40, 40), interp_rot);
        }
        else
        {
            // Draw mouse sprite
            mouse_sprite->DrawRotated(
                pos + ImVec2(per * x, per * y) +
                    ImVec2((per + BORDER_THICKNESS) / 2, (per + BORDER_THICKNESS) / 2),
                ImVec2(40, 40), mouse->Rot());
        }
    }
};

REGISTER_WINDOW(Maze)

}; // namespace Simulator::Windows
