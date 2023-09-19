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
    //! Load the texture for the SDL_Renderer from the image at path using stb_image
    Texture(SDL_Renderer *renderer, std::string path);
    ~Texture();

    //! Get the SDL_Surface for the Texture used to create the SDL_Texture
    inline SDL_Surface *Surface() { return surface; }
    //! Get the SDL_Texture holding the data of the Texture
    inline SDL_Texture *Tex() { return texture; }
    //! Get the width of the Texture in pixels
    inline int Width() { return width; }
    //! Get the height of the Texture in pixels
    inline int Height() { return height; }
    //! Draw the Texture at the center with the size and angle (degrees)
    void DrawRotated(ImVec2 center, ImVec2 size, float angle);

private:
    SDL_Surface *surface{nullptr};
    SDL_Texture *texture{nullptr};
    unsigned char *data{nullptr};

    int width, height, bpp;
};

} // namespace Simulator
