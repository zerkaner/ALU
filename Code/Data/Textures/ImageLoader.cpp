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
    return new SimpleTexture(data, x, y, cmp);
  }

  // Print error message.
  else printf("[ImageLoader] Error! Could not open file '%s'!\n", filename);
  return NULL;
}
