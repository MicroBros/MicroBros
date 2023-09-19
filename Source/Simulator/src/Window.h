#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>
#include <stdio.h>

#include "SimulatorMaze.h"
#include "Utils.h"

namespace Simulator
{

/*! \brief Window creation and main UI for simulator
 */
class Window
{
public:
    //! Open a window, pass absolute path to the executable launched
    Window(std::string executable);
    ~Window();

    //! Open and load the Maze and setup SimulatorMaze based on path
    void OpenMaze(std::string path);
    //! Run the Window event-loop blocking until closed
    void Run();

private:
    //! Draw the imgui UI
    void Draw(bool &done);
    //! Draw the top menu bar
    void DrawMenuBar(bool &done);
    //! Draw the imgui window displaying the Maze
    void DrawMazeWindow();
    //! Open the file picker to open a mazefile
    void OpenMaze();
    //! Show an error modal with error message
    void Error(std::string err);

    float dpi{1.0f};
    std::vector<std::string> algorithms;
    size_t algorithm{0};
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
    std::unique_ptr<Utils::Texture> mouse_sprite{nullptr};
    std::optional<std::function<void(void)>> error_setup{std::nullopt};
    std::optional<std::string> error{std::nullopt};
    std::unique_ptr<SimulatorMaze> maze{nullptr};
    ImGuiID error_popup{ImHashStr("ERROR_POPUP")};
};

} // namespace Simulator
