#pragma once
#include <stdio.h>


/** Converter class that offers conversions from and to external formats. */
class Converter {

  private:

    FILE* _fp = NULL;       // File pointer for input stream.
    const char* _savename;  // Path to output file (argument).


    /** Read a .glf binary font definition and output data as source code. */
    void ReadGLF();


    /** Load the file to convert. Detects file format and starts the responsible converter. 
     * @param filename Path to the file to open. */
    void StartConversion (const char* filename);


  public:

    /** Create converter and start file conversion. 
     * @param argc Argument counter. Has to be '2'.
     * @param argv Parameter array. Should consist of input and output paths. */
    Converter(int argc, char** argv);
};
