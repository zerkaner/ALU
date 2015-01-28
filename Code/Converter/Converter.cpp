#pragma warning(disable: 4996)
#include <Converter/Converter.h>
#include <Data/Primitives.h>
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

  // Try to open writer stream. Quit on failure.
  FILE* writer = fopen(_savename, "w");
  if (writer == NULL) {
    printf ("\nError writing to file \"%s\"!\n", _savename);
    return;
  }

  // both 24
  int i, e, s, w, h, t;
  Character ch;
  Character* p;

  fprintf(writer, "#include <Data/Primitives.h>\n");
  fprintf(writer, "\nFont CreateDefaultFont() {\n");
  fprintf(writer, "\n  // Font initialization.");
  fprintf(writer, "\n  Font defFont = { ");
  fread(&t, sizeof(uint), 1, _fp);  fprintf(writer, "%d, ", t);  // Texture ID (dummy).
  fread(&w, sizeof(int), 1, _fp);   fprintf(writer, "%d, ", w);  // Font width.
  fread(&h, sizeof(int), 1, _fp);   fprintf(writer, "%d, ", h);  // Font height.
  fread(&s, sizeof(int), 1, _fp);   fprintf(writer, "%d, ", s);  // First character.
  fread(&e, sizeof(int), 1, _fp);   fprintf(writer, "%d, ", e);  // Last character.
  fread(&p, sizeof(Character*), 1, _fp);                         // Skip the reference.
  fprintf(writer, "0 };\n");

  int nrChars = e - s + 1;    // Number of characters: End position - start position + 1.
  
  fprintf(writer, "  defFont.Characters = new Character[%d];\n\n", nrChars); 
  fprintf(writer, "  // Size and texture vector definition for each character:");
  for (int j = 0; j < nrChars; j ++) {
    fprintf(writer, "\n  defFont.Characters[%2d] = {", j);
    fread(&ch, sizeof(Character), 1, _fp);
		ch.width =  (int)((ch.tx2 - ch.tx1)*w); //| Perform calculation here.
		ch.height = (int)((ch.ty2 - ch.ty1)*h); //| (storage precision too low)
    fprintf(writer, "%d, ", ch.width);      // Character width.
    fprintf(writer, "%d, ", ch.height);     // Character height. 
    fprintf(writer, "%f, ", ch.tx1);        // Texture x1.  
    fprintf(writer, "%f, ", ch.ty1);        // Texture y1.  
    fprintf(writer, "%f, ", ch.tx2);        // Texture x2.  
    fprintf(writer, "%f",   ch.ty2);        // Texture y2.  
    fprintf(writer, " };");  
  }
  fprintf(writer, "\n  return defFont;\n}");


  // Parse input file and write output file.
  fprintf(writer, "\n\n//Texture definition (hexadecimal byte code):");
  fprintf(writer, "\nunsigned char defTexture[] = {");
  int c; i = 0;
  while ((c = fgetc (_fp)) != EOF) {
    if (i == 0) fprintf(writer, "\n  ");  // Indent new line by two spaces.
    fprintf(writer, "0x%02x, ", c);       // Output in hex (like 0x21). %d is also possible.
    if (++i == 16) i = 0;                 // Limit line length to 16. 
  }
  fprintf(writer, "\n};\n");

  // Print file length and close writer.
  printf ("Written %d bytes to file \"%s\".\n", ftell(writer), _savename);
  fclose(writer);
}
