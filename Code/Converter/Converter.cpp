#pragma warning(disable: 4996)
#include <Converter/Converter.h>
#include <string.h>


/** Create converter and start file conversion. 
  * @param argc Argument counter. Has to be '2'.
  * @param argv Parameter array. Should consist of input and output paths. */
Converter::Converter(int argc, char** argv) {
      
  // If parameter syntax is valid, start file conversion.
  if (argc == 2) {
    _savename = argv[1];
    StartConversion(argv[0]);
  }

  // Print syntax error.
  else {
    printf ("[Error] Invalid converter parameter syntax!\n"
            "Usage: ALU.exe --conv <inputfile> <outputfile>\n");
  }
}


/** Load the file to convert. Detects file format and starts the responsible converter. 
  * @param filename Path to the file to open. */
void Converter::StartConversion (const char* filename) {
  
  // File recognition information. 
  static const int nrBinary = 4;    // Number of binary formats (beginning from left).
  static const int nrFormats = 6;   // Number of available fileendings.
  enum FORMATS {GLF, MDX, MS3D, RAW, MDL, OBJ}; // Attention: Make sure 'fileendings' is equal! 
  const char* fileendings[nrFormats] = {".glf", ".mdx", ".ms3d", ".raw", ".mdl", ".obj"};


  // Detect file format.
  int fileformat = -1;   
  for (int i = 0; i < nrFormats; i ++) {
    if (strstr (filename, fileendings[i]) != NULL) {
      fileformat = i;
      break;
    }
  }

  // Print error and return on unrecognized file format.
  if (fileformat == -1) {
    printf ("\nError reading file \"%s\"! Please make sure it has a proper ending.\n"
            "Formats currently supported: .glf, .mdl, .mdx, .obj, .raw, .ms3d\n", filename);
    return;
  }

  // Try to open filestream.
  _fp = fopen (filename, (fileformat < nrBinary)? "rb" : "r");
  if (_fp == NULL) {
    printf ("\nError opening file \"%s\"!\n", filename);
    return;
  }

  // Get the length of the file and print status output.
  fseek (_fp, 0L, SEEK_END);
  long bytes = ftell (_fp);
  fseek (_fp, 0L, SEEK_SET);
  printf ("\nOpening file \"%s\" [%d bytes].\n", filename, bytes);


  // Execute corresponding converter function.
  switch (fileformat) {
    case GLF :  ReadGLF();  break;
    default: printf("Sorry, converter module is not implemented yet!\n");
  }

  // Close input file stream after conversion.
  fclose (_fp);
}


/** Utility method to check the beginning of a string. [internal] 
 * @param string The string to check.
 * @param prefix The beginning to search for.
 * @return '1', if true, '0' otherwise. */
int StartsWith (const char* string, const char* prefix) {
  size_t lenpre = strlen (prefix), lenstr = strlen (string);
  return (lenstr < lenpre)? 0 : (strncmp (prefix, string, lenpre) == 0);
}


/** Read a .glf binary font definition and output data as source code. */
void Converter::ReadGLF() {
  printf("GLF reader engaged!\n");
}
