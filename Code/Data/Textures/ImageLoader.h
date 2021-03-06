#pragma once
class Texture;

class ImageLoader {
  public:

    /** Load a texture from a file into a SDL_Surface object. */
    static Texture* LoadTexture(const char* filename);

    /** Direct image-to-GPU loading function. */
    static unsigned int LoadDirect(const char* filename);
};
