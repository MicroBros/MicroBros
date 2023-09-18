#pragma once

#include <string>

#include <SDL.h>
#include <imgui.h>

namespace Simulator::Utils
{

// clang-format off
/*! \brief Util to load images and turn them into SDL textures
 */
class Texture
{
public:
    Texture(SDL_Renderer *renderer, std::string path);
    ~Texture();

    inline SDL_Surface *Surface() { return surface; }
    inline SDL_Texture *Tex() { return texture; }
    inline int Width() { return width; }
    inline int Height() { return height; }

    void DrawRotated(ImVec2 center, ImVec2 size, float angle);

private:
    SDL_Surface *surface{nullptr};
    SDL_Texture *texture{nullptr};
    unsigned char *data{nullptr};

    int width, height, bpp;
};

} // namespace Simulator
