#pragma once

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>
#include <stdio.h>

#include <functional>
#include <memory>
#include <optional>

#include "SimulatorMaze.h"
#include "Utils.h"

namespace Simulator
{

class Window
{
public:
    Window(std::string executable);
    ~Window();
    void OpenMaze(std::string path);
    void Run();

private:
    void Draw(bool &done);
    void DrawMenuBar(bool &done);
    void DrawMazeWindow();
    void OpenMaze();
    void Error(std::string err);

    float dpi{1.0f};
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
    std::unique_ptr<Texture> mouse_sprite{nullptr};
    std::optional<std::function<void(void)>> error_setup{std::nullopt};
    std::optional<std::string> error{std::nullopt};
    std::unique_ptr<SimulatorMaze> maze{nullptr};
    ImGuiID error_popup{ImHashStr("ERROR_POPUP")};
};

} // namespace Simulator
