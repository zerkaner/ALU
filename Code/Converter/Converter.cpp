#pragma warning(disable: 4996)
#include <Converter/Converter.h>
#include <Data/Model3D.h>
#include <Data/Primitives.h>
#include <stdlib.h>
#include <string.h>


Converter::Converter(int argc, char** argv) {
      
  // If parameter syntax is valid, start file conversion.
  if (argc == 2) {
    _savename = argv[1];
    StartConversion(argv[0]);
  }  
  else {  // Print syntax error.
    printf ("[Error] Invalid converter parameter syntax!\n"
            "Usage: ALU.exe --conv <inputfile> <outputfile>\n");
  }
}



void Converter::StartConversion (const char* filename) {
  
  // File recognition information. 
  static const int nrBinary = 4;    // Number of binary formats (beginning from left).
  static const int nrFormats = 6;   // Number of available fileendings.
  enum FORMATS {GLF, MDX, MS3D, RAW, MDL, OBJ}; // Attention: Make sure 'fileendings' is equal! 
  const char* fileendings[nrFormats] = {".glf", ".mdx", ".ms3d", ".raw", ".mdl", ".obj"};

  // Detect file format.
  int fileformat = -1;   
  for (int i = 0; i < nrFormats; i ++) {
    if (strstr(filename, fileendings[i]) != NULL) {
      fileformat = i;
      break;
    }
  }

  // Print error and return on unrecognized file format.
  if (fileformat == -1) {
    printf("\nError reading file \"%s\"! Please make sure it has a proper ending.\n"
           "Formats currently supported: .glf, .mdl, .mdx, .obj, .raw, .ms3d\n", filename);
    return;
  }

  // Try to open filestream.
  _fp = fopen(filename, (fileformat < nrBinary)? "rb" : "r");
  if (_fp == NULL) {
    printf("\nError opening file \"%s\"!\n", filename);
    return;
  }

  // Get the length of the file and print status output.
  fseek(_fp, 0L, SEEK_END);
  long bytes = ftell(_fp);
  fseek(_fp, 0L, SEEK_SET);
  printf("\nOpening file \"%s\" [%d bytes].\n", filename, bytes);


  // Execute corresponding converter function.
  switch (fileformat) {
    case GLF: ReadGLF();  break;
    case OBJ: ReadOBJ();  break;
    case MDX: ReadMDX();  break;
    default: printf("Sorry, converter module is not implemented yet!\n");
  }

  // Close input file stream after conversion.
  fclose(_fp);
}



/** Utility method to check the beginning of a string. [internal] 
 * @param string The string to check.
 * @param prefix The beginning to search for.
 * @return '1', if true, '0' otherwise. */
int StartsWith (const char* string, const char* prefix) {
  size_t lenpre = strlen (prefix), lenstr = strlen (string);
  return (lenstr < lenpre)? 0 : (strncmp (prefix, string, lenpre) == 0);
}



void Converter::ReadMDX() {
  printf("MDX converter loaded.\n");
 
  // Create variables and 3D model.
  DWORD dbuf = 0x00;          // Read-in buffer.
  DWORD remaining;            // Geoset byte size.
  Model3D model = Model3D();  // Empty model.
  int i;
  int totalV = 0, totalN = 0, totalT = 0, totalG = 0;
  int idCounter = 0;

  // Skip file to geoset definitions and read in size.
  while (dbuf != 'SOEG') fread(&dbuf, sizeof(DWORD), 1, _fp);  
  fread(&remaining, sizeof(DWORD), 1, _fp);
  for (int g = 0; remaining > 0; g ++) {
    Geoset* geoset = new Geoset();
    geoset->enabled = true;
    geoset->id = idCounter;

    // Read and reduce remaining size.
    fread(&dbuf, sizeof(DWORD), 1, _fp);
    remaining -= dbuf;

    // Read vertices.
    fread(&dbuf, sizeof(DWORD), 1, _fp); // Skip 'VRTX'.
    fread(&geoset->nrV, sizeof(DWORD), 1, _fp);
    totalV += geoset->nrV;
    geoset->vertices = new Float3[geoset->nrV];
    for (i = 0; i < geoset->nrV; i ++) {
      fread(&geoset->vertices[i].X, sizeof(float), 1, _fp);
      fread(&geoset->vertices[i].Y, sizeof(float), 1, _fp);
      fread(&geoset->vertices[i].Z, sizeof(float), 1, _fp);
    }

    // Read normals.
    fread(&dbuf, sizeof(DWORD), 1, _fp); // Skip 'NRMS'.
    fread(&geoset->nrN,  sizeof(DWORD), 1, _fp);
    totalN += geoset->nrN;
    geoset->normals = new Float3 [geoset->nrN];
    for (i = 0; i < geoset->nrN; i ++) {
      fread(&geoset->normals[i].X, sizeof(float), 1, _fp);
      fread(&geoset->normals[i].Y, sizeof(float), 1, _fp);
      fread(&geoset->normals[i].Z, sizeof(float), 1, _fp);
    }

    // Scan to faces definition.
    while (dbuf != 'XTVP') fread(&dbuf, sizeof(DWORD), 1, _fp);  // Skip 'PVTX'.
    fread(&geoset->nrG, sizeof(DWORD), 1, _fp);
    geoset->nrG /= 3;  // Only triangles used! 
    totalG += geoset->nrG;
    geoset->geometries = (Geometry*) calloc(geoset->nrG, sizeof(Geometry));
    for (i = 0; i < geoset->nrG; i ++) {
      geoset->geometries[i].symIndices = true;
      fread(&geoset->geometries[i].vIdx[0], sizeof(WORD), 1, _fp);
      fread(&geoset->geometries[i].vIdx[1], sizeof(WORD), 1, _fp);
      fread(&geoset->geometries[i].vIdx[2], sizeof(WORD), 1, _fp);
    }
    fread(&dbuf, sizeof(DWORD), 1, _fp);  // Skip 'GNDX'.
    fread(&dbuf, sizeof(DWORD), 1, _fp);  // Read vertex group numbers.
    for (i = dbuf; i > 0; i --) fread(&dbuf, sizeof(BYTE), 1, _fp);

    // Skip to texture coordinates [UVBS].
    while (dbuf != 'SBVU') fread(&dbuf, sizeof(DWORD), 1, _fp);
    fread(&geoset->nrT, sizeof(DWORD), 1, _fp);
    totalT += geoset->nrT;
    geoset->textures = new Float2[geoset->nrT];
    for (i = 0; i < geoset->nrT; i ++) {
      fread(&geoset->textures[i].X, sizeof(float), 1, _fp);
      fread(&geoset->textures[i].Y, sizeof(float), 1, _fp);
    }

    // Add new geoset.
    model.Geosets.push_back(geoset);
    idCounter ++;

    // Scan to 'VRTX' (to skip the rubbish afterwards), then reset to proper start.
    if (remaining > 0) {
      while (dbuf != 'XTRV') fread(&dbuf, sizeof(DWORD), 1, _fp);
      fseek(_fp, -8, SEEK_CUR);
    }
  }

  // Print parser results.
  printf("Parser results:  \n"
         " - Geosets   : %d\n"
         " - Vertices  : %d\n"
         " - Normals   : %d\n"
         " - Textures  : %d\n"
         " - Geometries: %d\n", 
           model.Geosets.size(), totalV, totalN, totalT, totalG);

  // Write model to disk.
  model.WriteFile(_savename);
}



void Converter::ReadOBJ() {
  printf ("OBJ converter loaded.\n");

  int v, vn, vt;       // Index variables for faces match.
  char buffer [256];   // Input buffer for file read-in.
  char mtlfile [80];   // Material library file name.
  

  // First run: Determine geosets and array sizes.
  int geos = 1;
  LONG4* counters;    // Group member counters (zero is default).
  counters = (LONG4*) malloc(sizeof(LONG4)); 
  counters[0] = LONG4();
  while (fgets(buffer, 256, _fp) != NULL) {
  
    // A new group was found. Add counters for it.
    if (StartsWith(buffer, "g ")) {
      if (counters[geos-1].v != 0) { // Skip empty definitions.
        geos ++;
        counters = (LONG4*) realloc(counters, sizeof(LONG4) * geos);
        counters[geos-1] = LONG4();
      }
    }

    // Increase counter values.
    else if (StartsWith(buffer, "v "))  counters[geos-1].v ++;   
    else if (StartsWith(buffer, "vn ")) counters[geos-1].n ++;
    else if (StartsWith(buffer, "vt ")) counters[geos-1].t ++;
    else if (StartsWith(buffer, "f "))  counters[geos-1].g ++;
  }

  // Add for total numbers.
  long tV = 0, tN = 0, tT = 0, tG = 0;
  for (int i = 0; i < geos; i ++) {
    tV += counters[i].v;
    tN += counters[i].n;
    tT += counters[i].t;
    tG += counters[i].g;
  }

  // Allocate heap memory for first run, also create model.
  int curGeo = 0;
  Model3D model = Model3D();  
  Float3*   vertices   = (Float3*)   calloc(counters[curGeo].v, sizeof(Float3));
  Float3*   normals    = (Float3*)   calloc(counters[curGeo].n, sizeof(Float3));
  Float2* textures     = (Float2*)   calloc(counters[curGeo].t, sizeof(Float2));
  Geometry* geometries = (Geometry*) calloc(counters[curGeo].g, sizeof(Geometry));
  std::vector<Material*> materials = std::vector<Material*>();


  // Second run: Gather data and load them into structures.
  fseek (_fp, 0L, SEEK_SET);
  int curV = 0, curN = 0, curT = 0, curG = 0;   // Current index (loop counter).
  int foV = 1, foN = 1, foT = 1;                // Faces offsets (index dereferencing).
  while (fgets (buffer, 256, _fp) != NULL) {

    if (StartsWith(buffer, "v ")) {
      sscanf(buffer, "v %f %f %f", &vertices[curV].X, &vertices[curV].Y, &vertices[curV].Z);
      curV ++;
    }

    else if (StartsWith(buffer, "vn ")) {
      sscanf(buffer, "vn %f %f %f", &normals[curN].X, &normals[curN].Y, &normals[curN].Z);
      curN ++;
    }

    else if (StartsWith(buffer, "vt ")) {
      sscanf(buffer, "vt %f %f", &textures[curT].X, &textures[curT].Y);
      curT ++;
    }

    else if (StartsWith(buffer, "f ")) {    
      int count = 0;
      long* indices = NULL;

      // Loop over all entries and aggregate geometry components.
      char* splitPtr = strtok(buffer, " ");  // Split of first segment.      
      while (splitPtr != NULL) {  
        count ++;
        //if (count == 4) break;  // Break on invalid mesh.
        indices = (long*) realloc (indices, count * sizeof(long));

        // Dereference the vertices, normals and texture crap.
        if (sscanf(splitPtr, "%d/%d/%d", &v, &vt, &vn) == 3) {
          geometries[curG].vIdx [count-1] = v  - foV;
          geometries[curG].nIdx [count-1] = vn - foN;
          geometries[curG].tIdx [count-1] = vt - foT;      
          //TODO What if there are no n or t vrtx?
        }
        else if (sscanf(splitPtr, "%d//%d", &v, &vn) == 2) {
          geometries[curG].vIdx[count-1] = v  - foV;
          geometries[curG].nIdx[count-1] = vn - foN;
        }
        else if (sscanf(splitPtr, "%d", &v) == 1) {
          geometries[curG].vIdx[count-1] =  v - foV;
        }
        else count --;

        splitPtr = strtok (NULL, " ");
      } 
      curG ++;
    }

    // Load material (mtllib).
    else if (StartsWith(buffer, "mtllib ")) {
      sscanf(buffer, "mtllib %s", &mtlfile);
      //readMTL (mtlfile, &materials);
    }


    // If geoset group is finished, write it to model!
    if (curV == counters[curGeo].v && curN == counters[curGeo].n && 
        curT == counters[curGeo].t && curG == counters[curGeo].g) {
      
      Geoset* geoset = new Geoset();
      geoset->id = curGeo;
      geoset->enabled = true;
      geoset->nrV = counters[curGeo].v;
      geoset->nrN = counters[curGeo].n;
      geoset->nrT = counters[curGeo].t;
      geoset->nrG = counters[curGeo].g;
      geoset->vertices = vertices;
      geoset->normals = normals;
      geoset->textures = textures;
      geoset->geometries = geometries;
      model.Geosets.push_back(geoset);
      
      curGeo ++;
      foV += curV; foN += curN; foT += curT;  // Increase faces offset
      curV = 0; curN = 0; curT = 0; curG = 0; // Reset counters.

      // Still geosets left? Re-allocate memory then!
      if (curGeo < geos) {
        vertices   = (Float3*)   calloc(counters[curGeo].v, sizeof(Float3));
        normals    = (Float3*)   calloc(counters[curGeo].n, sizeof(Float3));
        textures   = (Float2*)   calloc(counters[curGeo].t, sizeof(Float2));
        geometries = (Geometry*) calloc(counters[curGeo].g, sizeof(Geometry));    
      }
    }
  }

  // Print parser results.
  printf ("Parser results:  \n"
          " - Geosets   : %d\n"
          " - Vertices  : %d\n"
          " - Normals   : %d\n"
          " - Materials : %d\n"
          " - Tex Coords: %d\n"
          " - Geometries: %d\n", 
            geos, tV, tN, materials.size(), tT, tG);

  // Write model to disk.
  model.WriteFile(_savename);
}



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

  // Close output file stream.
  printf ("Written %d bytes to file \"%s\".\n", ftell(writer), _savename);  
  fclose(writer);
}
