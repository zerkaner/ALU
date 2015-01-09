#include "TextWriter.h"
#include <stdio.h>


GLFontBase::GLFontBase() : ok(false) {}

void GLFontBase::CreateImpl(char* filename, GLuint Tex) {
	Font.Char = NULL;
	FreeResources();

	FILE* file = fopen(filename, "rb");
  if (file == NULL) return;
		
	fread(&Font, sizeof(GLFONT), 1, file);            // Read glFont structure
	Font.Tex = Tex;                                   // Save texture number
	int Num = Font.IntEnd - Font.IntStart + 1;        // Get number of characters
	Font.Char = new GLFONTCHAR[Num];	                // Allocate memory for characters
	fread(Font.Char, sizeof(GLFONTCHAR), Num, file);	// Read glFont characters
	Num = Font.TexWidth * Font.TexHeight * 2;	        // Get texture size
	char *TexBytes = new char[Num];	                  // Allocate memory for texture data
	fread(TexBytes, sizeof(char), Num, file);	        // Read texture data

	//Set texture attributes
	glBindTexture(GL_TEXTURE_2D, Font.Tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Create texture
	glTexImage2D(GL_TEXTURE_2D, 0, 2, Font.TexWidth, Font.TexHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (void *)TexBytes);

	//Clean up

	delete []TexBytes;
	fclose(file);

	ok = true;
}


void GLFontBase::FreeResources () {
	if (Font.Char != NULL) delete []Font.Char;
	ok = false;
}


void GLFontBase::Begin () {
	if (ok)	glBindTexture(GL_TEXTURE_2D, Font.Tex);
}


GLFontBase::~GLFontBase () {
	FreeResources();
}


PixelPerfectGLFont::PixelPerfectGLFont() {}


void PixelPerfectGLFont::Create(char* filename, GLuint Tex) {
	GLFontBase::CreateImpl(filename, Tex);
	for (int i = 0; i < Font.IntEnd - Font.IntStart + 1; i++)	{
		Font.Char[i].width =  (int)((Font.Char[i].tx2 - Font.Char[i].tx1)*Font.TexWidth);
		Font.Char[i].height = (int)((Font.Char[i].ty2 - Font.Char[i].ty1)*Font.TexHeight);
	}
}


void PixelPerfectGLFont::TextOut (char* String, int x, int y, int z) {
	//Return if we don't have a valid glFont
	if (!ok) return;

	//Begin rendering quads
	glBegin(GL_QUADS);

	//Loop through characters
	for (int i = 0; String[i] != '\0'; i++) {
		//Get pointer to glFont character
		GLFONTCHAR *Char = &Font.Char[(int)String[i] - Font.IntStart];

		//Specify vertices and texture coordinates
		glTexCoord2f(Char->tx1, Char->ty1);
		glVertex3i(x, y, z);
		glTexCoord2f(Char->tx1, Char->ty2);
		glVertex3i(x, y + Char->height, z);
		glTexCoord2f(Char->tx2, Char->ty2);
		glVertex3i(x + Char->width, y + Char->height, z);
		glTexCoord2f(Char->tx2, Char->ty1);
		glVertex3i(x + Char->width, y, z);

		//Move to next character
		x += Char->width;
	}

	//Stop rendering quads
	glEnd();
}
