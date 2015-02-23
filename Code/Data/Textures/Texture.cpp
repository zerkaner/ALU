#include "Texture.h"
#include <SDL.h>
#include <SDL_opengl.h>


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



SimpleTexture::SimpleTexture(int width, int height) {
  _id = 0;
  _width = width;
  _height = height;
  if (SDL_GL_ExtensionSupported("GL_ARB_texture_non_power_of_two")) {
    _storedWidth  = _width;
    _storedHeight = _height;
  } 
  else {  // Extend storage size, if it must be a power of two.
    _storedWidth  = nextPowerOfTwo(_width);
    _storedHeight = nextPowerOfTwo(_height);
  }
}



SimpleTexture::~SimpleTexture () {
  if (_id) glDeleteTextures(1, &_id);
}



bool SimpleTexture::InitFromSurface(SDL_Surface* surface) {
  if (surface->w == _width && surface->h == _height) {

    // Get RGB / BGR format (little or big endian).  
    unsigned int format;
    bool isLE = (surface->format->Rmask == 0x000000ff);
    switch (surface->format->BytesPerPixel) {     
      case 3 : format = isLE? GL_RGB  : GL_BGR;  break;
      case 4 : format = isLE? GL_RGBA : GL_BGRA; break;
      default: return false;
    }    
    
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_id);

    // Set texture attributes.
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
 
    // Create texture.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _storedWidth, _storedHeight,
                 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    return true;
  }
  return false;
}



TextureSlice::TextureSlice(Texture* parent, int x, int y, int width, int height) {
  _parent = parent;
  _offsetX = x;
  _offsetY = y;
  _width = width;
  _height = height;
}



Texture* TextureFromSurface(SDL_Surface* surface) {
  SimpleTexture* sTex = new SimpleTexture(surface->w, surface->h);
  sTex->InitFromSurface(surface);
  return sTex;
}
