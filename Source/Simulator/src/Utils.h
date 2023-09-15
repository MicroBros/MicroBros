#pragma once

#include <SDL.h>

#include <string>

class Texture
{
public:
    Texture(SDL_Renderer *renderer, std::string path);
    ~Texture();

    inline SDL_Surface *Surface() { return surface; }
    inline SDL_Texture *Tex() { return texture; }
    inline int Width() { return width; }
    inline int Height() { return height; }

private:
    SDL_Surface *surface{nullptr};
    SDL_Texture *texture{nullptr};
    unsigned char *data{nullptr};

    int width, height, bpp;
};
