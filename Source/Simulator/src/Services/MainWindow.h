#pragma once

#include <memory>
#include <optional>

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>

#include "../Utils.h"
#include "Service.h"

namespace Simulator::Services
{

/*! \brief MainWindow containing platform window using SDL, and main imgui loop
 */
class MainWindow : public Service
{
public:
    MainWindow(Application *application, std::string executable);
    ~MainWindow();

    void Tick();

    inline Utils::Texture *GetMouseSprite() noexcept { return mouse_sprite.get(); }

private:
    //! Draw imgui
    void Draw();
    //! Draw the top menu bar
    void DrawMenuBar();

    Application *application{nullptr};

    float dpi{1.0f};
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
    std::unique_ptr<Utils::Texture> mouse_sprite{nullptr};
    ImGuiID error_popup{ImHashStr("ERROR_POPUP")};
};

} // namespace Simulator::Services
