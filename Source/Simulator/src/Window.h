#pragma once

#include <functional>
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <optional>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl2.h>

#include "SimulatorMaze.h"

class Window
{
public:
    Window();
    ~Window();
    void OpenMaze(std::string path);
    void Run();

private:
    void Draw(bool &done);
    void DrawMenuBar(bool &done);
    void DrawMazeWindow();
    void OpenMaze();
    void Error(std::string err);

    SDL_Window *window{nullptr};
    SDL_GLContext gl_context;
    std::optional<std::function<void(void)>> error_setup{std::nullopt};
    std::optional<std::string> error{std::nullopt};
    std::unique_ptr<SimulatorMaze> maze{nullptr};
    ImGuiID error_popup{ImHashStr("ERROR_POPUP")};
};
