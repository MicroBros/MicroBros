#include <fmt/format.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

#include "Utils.h"

Texture::Texture(SDL_Renderer *renderer, std::string path)
{
    // Load uncompressed data of image
    data = stbi_load(path.c_str(), &width, &height, &bpp, 0);
    if (!data)
        throw std::runtime_error(fmt::format("Error loading stbi image  {}", path));

    // Calculate the pitch per line using width and bytes per pixel
    int pitch{width * bpp};

    // Create the SDL surface and turn it into a SDL texture
    surface = SDL_CreateSurfaceFrom(data, width, height, pitch,
                                    bpp == 3 ? SDL_PIXELFORMAT_XBGR8888 : SDL_PIXELFORMAT_ABGR8888);
    if (!surface)
        throw std::runtime_error(
            fmt::format("Error creating SDL surface for \"{}\": {}", path, SDL_GetError()));

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
        throw std::runtime_error(
            fmt::format("Error creating SDL texture for \"{}\": {}", path, SDL_GetError()));
}

Texture::~Texture()
{
    if (texture)
        SDL_DestroyTexture(texture);
    if (surface)
        SDL_DestroySurface(surface);
    if (data)
        delete data;
}
