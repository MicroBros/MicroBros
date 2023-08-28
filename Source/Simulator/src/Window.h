#pragma once

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

class Window
{
public:
    Window();
    ~Window();

private:
    SDL_Window *window{nullptr};
    SDL_GLContext gl_context;
};
