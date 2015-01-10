#pragma warning(disable: 4996) // Skip fopen warnings.

#include <UserInterface/TextWriter.h>

#include <stdio.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>


/** Initialize a new text writer with the given font.
  * @param filename Path to the font file to load. */
TextWriter::TextWriter(char* filename) {

  FILE* file = fopen(filename, "rb");          // Open file stream.
  fread(&_font, sizeof(Font), 1, file);        // Read in font structure.
  glGenTextures(1, &_font.Texture);            // Generate OpenGL texture number.     
  int nrChars = _font.End - _font.Start + 1;   // Get number of characters.
  _font.Characters = new Character[nrChars];   // Allocate memory for characters.
  fread(_font.Characters, sizeof(Character), nrChars, file);	// Read in characters.     
  int size = _font.Width * _font.Height * 2;	 // Get texture size.
  char* textures = new char[size];             // Allocate memory for texture data.
  fread(textures, sizeof(char), size, file);   // Read texture data from file.
  fclose(file);                                // Close file stream.

	// Set texture attributes.
	glBindTexture(GL_TEXTURE_2D, _font.Texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Create texture.
	glTexImage2D(GL_TEXTURE_2D, 0, 2, _font.Width, _font.Height, 0, 
                GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (void*) textures);
  delete[] textures;

  // Pixel-precise character alignment.
	for (int i = 0; i < nrChars; i ++)	{
		_font.Characters[i].width =  (int)((_font.Characters[i].tx2 - _font.Characters[i].tx1)*_font.Width);
		_font.Characters[i].height = (int)((_font.Characters[i].ty2 - _font.Characters[i].ty1)*_font.Height);
	}
}


/** Destroy text writer and free allocated memory. */
TextWriter::~TextWriter() {
  delete[] _font.Characters;
}


/** Write some text to the screen.
  * @param text The text to output. 
  * @param x X coordinate on screen.
  * @param y Y coordinate on screen. */
void TextWriter::WriteText(char* text, int x, int y) {
   
  // Enable blending (transparent overlay) and font texture.  
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_DST_COLOR);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, _font.Texture);
      
  // Use quad mode to draw each character.
  glBegin(GL_QUADS);
  for (int i = 0; text[i] != '\0'; i ++) {
        
    // Calculate pointer to the char to print.
    Character* c = &_font.Characters[(int)text[i] - _font.Start]; 
		
		// Specify vertices and texture coordinates.
		glTexCoord2f(c->tx1, c->ty1);	  glVertex2i(x, y);
		glTexCoord2f(c->tx1, c->ty2);   glVertex2i(x, y + c->height);
		glTexCoord2f(c->tx2, c->ty2);   glVertex2i(x + c->width, y + c->height);
		glTexCoord2f(c->tx2, c->ty1);	  glVertex2i(x + c->width, y);

		//Move to next character.
		x += c->width;
  }
  glEnd();

  // Disable blending and texture.
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}
