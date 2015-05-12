#pragma warning(disable: 4996) // Skip MSVC warnings.
#include "ImageLoader.h"
#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>


Texture* ImageLoader::LoadTexture(const char* filename) { 
  FILE* fp = fopen(filename, "rb");
  if (fp != NULL) {

    fseek(fp, 0L, SEEK_END);
    unsigned long rawSize = ftell(fp);
    unsigned char* rawData = new unsigned char [rawSize];
    fseek(fp, 0L, SEEK_SET);
    fread(rawData, sizeof (unsigned char), rawSize, fp);
    fclose(fp);

    return new SimpleTexture(rawData, rawSize, filename);
  }

  // Print error message.
  else printf("[ImageLoader] Error! Could not open file '%s'!\n", filename);
  return NULL;
}
