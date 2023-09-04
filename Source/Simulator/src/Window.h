#pragma once

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <optional>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

class Window
{
public:
    Window();
    ~Window();

private:
    void DrawMenuBar(bool &done);
    void OpenMaze();
    void OpenMaze(std::string path);
    void Error(std::string err);

    SDL_Window *window{nullptr};
    SDL_GLContext gl_context;
    std::optional<std::string> error{std::nullopt};
    ImGuiID error_popup{ImHashStr("ERROR_POPUP")};
};
