#include "Texture.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Function for examining the next "power-of-two". */
template<class T>
T nextPowerOfTwo(T value) {
  if ((value & (value - 1)) == 0) return value;
  value -= 1;
  for (size_t i = 1; i < sizeof(T)*8; i <<= 1) value = value|value>>i;
  return value + 1;
}



Texture* Texture::Slice(int x, int y, int width, int height) {
  return new TextureSlice(this, x, y, width, height);
}



SimpleTexture::SimpleTexture(unsigned char* data, int resX, int resY, int cmp) {

  // Take care of the byteorder for RGB-masks.
  uint32_t rmask, gmask, bmask, amask;
  bool isLE;
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      rmask = 0xff000000;
      gmask = 0x00ff0000;
      bmask = 0x0000ff00;
      amask = 0x000000ff;
      isLE = false;
  #else
      rmask = 0x000000ff;
      gmask = 0x0000ff00;
      bmask = 0x00ff0000;
      amask = 0xff000000;
      isLE = true;
  #endif


  // Create the surface and determine RGB(A) / BGR(A) format (OpenGL code).  
  SDL_Surface* sf;
  unsigned int format;
  switch (cmp) {
    
    case 3 : 
      sf = SDL_CreateRGBSurface(0, resX, resY, 24, rmask, gmask, bmask, 0); 
      format = isLE? GL_RGB : GL_BGR;
      break;
    
    case 4 : 
      sf = SDL_CreateRGBSurface(0, resX, resY, 32, rmask, gmask, bmask, amask); 
      format = isLE? GL_RGBA : GL_BGRA;
      break;
    
    default: 
      free(data); 
      printf("[Texture] Error! Texture has invalid components (%d)!\n", cmp);
      return;
  }
    

  // Copy pixels to surface object and set members. 
  memcpy(sf->pixels, data, cmp * resX * resY);
  _data = data;
  _components = cmp;

  // Create an OpenGL texture from the SDL surface.
  _id = 0;
  _width = sf->w;
  _height = sf->h;
  if (SDL_GL_ExtensionSupported("GL_ARB_texture_non_power_of_two")) {
    _storedWidth  = _width;
    _storedHeight = _height;
  } 
  else {  // Extend storage size, if it must be a power of two.
    _storedWidth  = nextPowerOfTwo(_width);
    _storedHeight = nextPowerOfTwo(_height);
  }
 

  // Set texture attributes.
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
 
  // Create texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _storedWidth, _storedHeight,
                0, format, GL_UNSIGNED_BYTE, sf->pixels);
}



SimpleTexture::~SimpleTexture () {
  if (_id) glDeleteTextures(1, &_id);
  free(_data);
}



TextureSlice::TextureSlice(Texture* parent, int x, int y, int width, int height) {
  _parent = parent;
  _offsetX = x;
  _offsetY = y;
  _width = width;
  _height = height;
}
