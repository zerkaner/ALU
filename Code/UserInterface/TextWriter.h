#pragma once

// Skip fopen warnings.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <SDL_opengl.h>
#include <gl/GLU.h>


class GLFontBase {
  public:
	  GLFontBase();
	  void Begin();
	  virtual ~GLFontBase();

  protected:

	  void CreateImpl(char* filename, GLuint Tex);

	  typedef struct {
	    union { float dx; int width;  };
	    union { float dy; int height; };
	    float tx1, ty1;
	    float tx2, ty2;
	  } GLFONTCHAR;

	  typedef struct {
	    int Tex;
	    int TexWidth, TexHeight;
	    int IntStart, IntEnd;
	    GLFONTCHAR *Char;
	  } GLFONT;

	  GLFONT Font;
	  bool ok;

  private:
	  void FreeResources();
};


class PixelPerfectGLFont : public GLFontBase {
public:
	PixelPerfectGLFont();
	void Create(char* filename, GLuint Tex);
	void TextOut (char* String, int x, int y, int z);
};
