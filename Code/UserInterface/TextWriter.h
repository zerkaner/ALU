/* DISCLAIMER: The code used in this class is based on "glFont".  */
/* It also uses its file format to load generated fonts.          */
/* - Copyright (c) 1998 Brad Fish                                 */
/* - E-mail: bhf5@email.byu.edu                                   */
/* - Web: http://students.cs.byu.edu/~bfish/                      */

#pragma once
#include <Data/Primitives.h>


/** A simple text writer. It is capable of rendering texts to a OpenGL surface. */
class TextWriter {

  private:

    Font _font;  // Font assigned to this writer.

    /** Initialize the built-in font. */
    void InitializeDefaultFont();


    /** Set up the font texture.
     * @param texture The texture array. */
    void InitializeTexture(unsigned char* texture);


  public:

    /** Initialize a new text writer with the given font.
     * @param filename Path to the font file to load. */
    TextWriter(char* filename);


    /** Initialize a text writer with the inbuilt font. */
    TextWriter();


    /** Destroy text writer and free allocated memory. */
    ~TextWriter();


    /** Write some text to the screen.
     * @param text The text to output. 
     * @param x X coordinate on screen.
     * @param y Y coordinate on screen. */
    void WriteText(char* text, int x, int y);
};
