#pragma warning(disable: 4996) // Skip MSVC warnings.
#include "ImageLoader.h"
#include "stb_image.h"
#include <SDL_opengl.h>
#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>


/** Load texture from file to internal structure. */
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


/** Direct image-to-GPU loading function. */
unsigned int ImageLoader::LoadDirect(const char* filename) {
  GLuint texture_id;
  int resX, resY, cmp;
  GLubyte* imgdata = stbi_load(filename, &resX, &resY, &cmp, 0);
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(
    GL_TEXTURE_2D,    // GL storage target.
    0,                // Level 0 (Base image, not mipmap).
    GL_RGBA,          // Internal format.
    resX,             // Texture width.
    resY,             // Texture height.
    0,                // No border (always 0 in OpenGL ES).
    GL_RGBA,          // Format.
    GL_UNSIGNED_BYTE, // Input type.
    imgdata           // Pointer to input.
  );
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture_id;
}