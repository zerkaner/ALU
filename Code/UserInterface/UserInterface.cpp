#include <UserInterface/UserInterface.h>

#include <SDL_opengl.h>

//DBG TEST
#include <UserInterface/TextWriter.h>
PixelPerfectGLFont font = PixelPerfectGLFont();
GLuint tex;


UserInterface::UserInterface() { 
	glGenTextures(1, &tex);
  font.Create("..\\Output\\cali10", tex);
}


void UserInterface::Update() {

  // Enable blending (transparent overlay) and font texture.  
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_DST_COLOR);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex);
   
  // Font output.
  font.Begin();
  font.TextOut("Hello World (from UI)!", 10, 50, 0);

  // Disable blending and texture.
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

}