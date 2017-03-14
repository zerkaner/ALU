#include <Converter/Converter.h>


/** Read a .glf binary font definition and output data as source code.
 * @param inputfile The file to load.
 * @param savename The name to save the output file at. */
void Converter::ReadGlf(const char* inputfile, const char* savename) {
  printf("GLF reader engaged!\n");
  FILE* fp = FileUtils::File_open(inputfile, false);
  if (fp == NULL) return;

  // Try to open writer stream. Quit on failure.
  FILE* writer = fopen(savename, "w");
  if (writer == NULL) {
    printf ("\nError writing to file '%s'!\n", savename);
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
  fread(&t, sizeof(uint), 1, fp);  fprintf(writer, "%d, ", t);  // Texture ID (dummy).
  fread(&w, sizeof(int), 1, fp);   fprintf(writer, "%d, ", w);  // Font width.
  fread(&h, sizeof(int), 1, fp);   fprintf(writer, "%d, ", h);  // Font height.
  fread(&s, sizeof(int), 1, fp);   fprintf(writer, "%d, ", s);  // First character.
  fread(&e, sizeof(int), 1, fp);   fprintf(writer, "%d, ", e);  // Last character.
  fread(&p, sizeof(Character*), 1, fp);                         // Skip the reference.
  fprintf(writer, "0 };\n");

  int nrChars = e - s + 1;    // Number of characters: End position - start position + 1.

  fprintf(writer, "  defFont.Characters = new Character[%d];\n\n", nrChars);
  fprintf(writer, "  // Size and texture vector definition for each character:");
  for (int j = 0; j < nrChars; j ++) {
    fprintf(writer, "\n  defFont.Characters[%2d] = {", j);
    fread(&ch, sizeof(Character), 1, fp);
		ch.width =  (int)((ch.tx2 - ch.tx1)*w); //| Perform calculation here.
		ch.height = (int)((ch.ty2 - ch.ty1)*h); //| (storage precision too low)
    fprintf(writer, "%d, ", ch.width);      // Character width.
    fprintf(writer, "%d, ", ch.height);     // Character height.
    fprintf(writer, "%ff, ", ch.tx1);       // Texture x1.
    fprintf(writer, "%ff, ", ch.ty1);       // Texture y1.
    fprintf(writer, "%ff, ", ch.tx2);       // Texture x2.
    fprintf(writer, "%ff",   ch.ty2);       // Texture y2.
    fprintf(writer, " };");
  }
  fprintf(writer, "\n  return defFont;\n}");


  // Parse input file and write output file.
  fprintf(writer, "\n\n//Texture definition (hexadecimal byte code):");
  fprintf(writer, "\nunsigned char defTexture[] = {");
  int c; i = 0;
  while ((c = fgetc (fp)) != EOF) {
    if (i == 0) fprintf(writer, "\n  ");  // Indent new line by two spaces.
    fprintf(writer, "0x%02x, ", c);       // Output in hex (like 0x21). %d is also possible.
    if (++i == 16) i = 0;                 // Limit line length to 16.
  }
  fprintf(writer, "\n};\n");

  // Close in- and output file stream.
  printf ("Written %lu bytes to file '%s'.\n", ftell(writer), savename);
  fclose(writer);
  fclose(fp);
}
