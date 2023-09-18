#define _USE_MATH_DEFINES
#include <cmath>
#include <stdexcept>

#include <fmt/format.h>
#include <imgui_internal.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Utils.h"

namespace Simulator::Utils
{

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

// Based on example by ocornut on imgui issuetracker:
// https://github.com/ocornut/imgui/issues/1982#issuecomment-408834301
void Texture::DrawRotated(ImVec2 center, ImVec2 size, float angle)
{
    float rad{angle * (static_cast<float>(M_PI) / 180)};
    float cos{std::cos(rad)};
    float sin{std::sin(rad)};

    ImVec2 pos[4] = {center + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos, sin),
                     center + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos, sin),
                     center + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos, sin),
                     center + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos, sin)};

    ImGui::GetWindowDrawList()->AddImageQuad((ImTextureID)Tex(), pos[0], pos[1], pos[2], pos[3]);
}

} // namespace Simulator::Utils
