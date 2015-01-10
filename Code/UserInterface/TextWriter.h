/* DISCLAIMER: The code used in this class is based on "glFont".  */
/* It also uses its file format to load generated fonts.          */
/* - Copyright (c) 1998 Brad Fish                                 */
/* - E-mail: bhf5@email.byu.edu                                   */
/* - Web: http://students.cs.byu.edu/~bfish/                      */

#pragma once


/** A simple text writer. It is capable of rendering texts to a OpenGL surface. */
class TextWriter {

  private:

    // Character structure.
	  struct Character {
	    union { float dx; int width;  };
	    union { float dy; int height; };
	    float tx1, ty1;
	    float tx2, ty2;
	  };

    // Font structure.
	  struct Font {
	    unsigned int Texture;
	    int Width, Height;
	    int Start, End;
	    Character* Characters;
	  };

    Font _font;  // Font assigned to this writer.


  public:

    /** Initialize a new text writer with the given font.
     * @param filename Path to the font file to load. */
    TextWriter(char* filename);


    /** Destroy text writer and free allocated memory. */
    ~TextWriter();


    /** Write some text to the screen.
     * @param text The text to output. 
     * @param x X coordinate on screen.
     * @param y Y coordinate on screen. */
    void WriteText(char* text, int x, int y);
};
