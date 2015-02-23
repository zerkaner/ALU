#include "ImageLoader.h"
#include "Texture.h"
#include "stb_image.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Texture* ImageLoader::LoadTexture(const char* filename) { 
  FILE* fp = fopen(filename, "rb");
  if (fp != NULL) {
    int x, y, cmp;  // Stores x, y resolution and components.
    unsigned char* data = stbi_load_from_file(fp, &x, &y, &cmp, 0);
    fclose(fp);

    // Take care of the byteorder for RGB-masks.
    uint32_t rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    // Create the surface.
    SDL_Surface* sf;
    switch (cmp) {
      case 3 : sf = SDL_CreateRGBSurface(0, x, y, 24, rmask, gmask, bmask, 0);     break;
      case 4 : sf = SDL_CreateRGBSurface(0, x, y, 32, rmask, gmask, bmask, amask); break;
      default: stbi_image_free(data); return NULL;
    }
    // Copy pixels to surface object. 
    memcpy(sf->pixels, data, cmp * x * y);
    stbi_image_free(data);

    // Create an OpenGL texture from the SDL_Surface.
    Texture* texture = TextureFromSurface(sf);
    // delete sf; Heap exception, when freed.
    return texture;
  }

  // Print error message.
  else {
    printf("[ERROR]: Could not open file '%s'!\n", filename);
    return NULL;
  }
}