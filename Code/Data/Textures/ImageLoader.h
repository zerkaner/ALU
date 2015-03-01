#pragma once
class Texture;

class ImageLoader {
  public:

    /** Load a texture from a file into a SDL_Surface object. */
    static Texture* LoadTexture(const char* filename);
};
