#pragma warning(disable: 4996)
#include <Converter/Converter.h>
#include <Data/Model3D.h>
#include <Data/Primitives.h>
#include <Data/Textures/Texture.h>
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
    printf("\nError reading file '%s'! Please make sure it has a proper ending.\n"
           "Formats currently supported: .glf, .mdl, .mdx, .obj, .raw, .ms3d\n", filename);
    return;
  }

  // Try to open filestream.
  _fp = fopen(filename, (fileformat < nrBinary)? "rb" : "r");
  if (_fp == NULL) {
    printf("\nError opening file '%s'!\n", filename);
    return;
  }

  // Get the length of the file and print status output.
  fseek(_fp, 0L, SEEK_END);
  unsigned long bytes = ftell(_fp);
  fseek(_fp, 0L, SEEK_SET);
  printf("\nOpening file '%s' [%lu bytes].\n", filename, bytes);


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
  DWORD dbuf = 0x00;          // "Read-in and discard" buffer.
  DWORD remaining;            // Geoset byte size.
  DWORD chunkSize;            // Work storage for various chunk size read-ins.
  Model3D model = Model3D();  // Empty model.
  float fbuf;
  int i;
  int totalV = 0, totalN = 0, totalT = 0, totalG = 0;
  int idCounter = 0;


  //_____________________________________________________________________________________[SEQUENCES]
  // Scan for 'SEQS' - animation sequence codeblock.
  while (dbuf != 'SQES') fread(&dbuf, sizeof(DWORD), 1, _fp);
  fread(&chunkSize, sizeof(DWORD), 1, _fp);   // Read total sequence chunk size.
  for (unsigned int i = 0; i < chunkSize/132; i ++) {
    Sequence* seq = new Sequence();
    fread(&seq->Name,          sizeof(char), 80, _fp);  // Read sequence name.
    fread(&seq->IntervalStart, sizeof(DWORD), 1, _fp);  // Read first animation frame.
    fread(&seq->IntervalEnd,   sizeof(DWORD), 1, _fp);  // Read last animation frame.
    fread(&seq->MoveSpeed,     sizeof(float), 1, _fp);  // Read movement speed.
    fread(&dbuf,               sizeof(DWORD), 1, _fp);  // Read flags (evaluation later). 
    if (dbuf == 0) seq->Loop = true;                    // Flags: 0: Looping, 1: NonLooping.                 
    fread(&fbuf,               sizeof(float), 1, _fp);  // Read 'Rarity' (whatever it is).
    fread(&dbuf,               sizeof(DWORD), 1, _fp);  // Read 'SyncPoint' (probably n.y.i). 
    fread(&seq->BoundsRadius,  sizeof(float), 1, _fp);  // Read bounds radius (broad phase CD).
    fread(&seq->MinimumExtent, sizeof(Float3), 1, _fp); // Read 'Rarity' (whatever it is).
    fread(&seq->MaximumExtent, sizeof(Float3), 1, _fp); // Read 'Rarity' (whatever it is).
    model.Sequences.push_back(seq);
  }


  //_____________________________________________________________________________________[MATERIALS]
  // Scan for 'MTLS' - materials codeblock.
  while (dbuf != 'SLTM') fread(&dbuf, sizeof(DWORD), 1, _fp);
  fread(&chunkSize, sizeof(DWORD), 1, _fp);   // Read total material chunk size.
  DWORD inclSize;                             // Material inclusive size storage.
  std::vector<DWORD> texIDs;

  // Read all materials.
  for (unsigned int i = 0; i < chunkSize; i += inclSize) {
    fread(&inclSize, sizeof(DWORD), 1, _fp);  // Read 'InclusiveSize'.
    fread(&dbuf, sizeof(DWORD), 1, _fp);      // 'Priority Plane'.
    fread(&dbuf, sizeof(DWORD), 1, _fp);      // 'Flags'.      
    fread(&dbuf, sizeof(DWORD), 1, _fp);      // Skip the 'LAYS' keyword.
    DWORD nrLayers;                           // Number of layers following.
    fread(&nrLayers, sizeof(DWORD), 1, _fp);  // Read number of layers.

    // Read all layers of current material.
    for (unsigned int l = 0; l < nrLayers; l ++) {
      DWORD lInclSize;
      fread(&lInclSize, sizeof(DWORD), 1, _fp); // Read layer inclusive size.
      fread(&dbuf, sizeof(DWORD), 1, _fp);      // 'FilterMode'.
      fread(&dbuf, sizeof(DWORD), 1, _fp);      // 'ShadingFlags'.
      fread(&dbuf, sizeof(DWORD), 1, _fp);      // 'TextureID'.
      texIDs.push_back(dbuf);                   // Save the texture ID for later association.

      // Because we don't need all the following stuff at the moment, we just skip it.
      long remaining = lInclSize - 4*sizeof(DWORD);
      fseek(_fp, remaining, SEEK_CUR);
      //fread(&dbuf, sizeof(DWORD), 1, _fp);      // 'TextureAnimationID'.
      //fread(&dbuf, sizeof(DWORD), 1, _fp);      // 'CoordID'.
      //fread(&dbuf, sizeof(float), 1, _fp);      // 'Alpha'.
      // Read material alpha structure.
      // Read material texture ID structure.    
    }
  }


  //_____________________________________________________________________________________[TEXTURES]
  // Scan for 'TEXS' - textures codeblock.
  while (dbuf != 'SXET') fread(&dbuf, sizeof(DWORD), 1, _fp);
  fread(&chunkSize, sizeof(DWORD), 1, _fp);

  // Iterate over all texture paths.
  for (unsigned int t = 0; t < chunkSize/268; t ++) {
    fread(&dbuf, sizeof(DWORD), 1, _fp);      // Read (and discard) 'Replaceable ID'.    
    char texPath[260];
    fread(&texPath, sizeof(char), 260, _fp);  // Read texture path.
    if(texPath[0] != '\0') {
      printf("Loading texture '%s' ", texPath);
      
      // Try to open file stream.
      FILE* texReader = fopen(texPath, "rb");
      if (texReader != NULL) {
        fseek(texReader, 0L, SEEK_END);          //| Read image size
        unsigned long bytes = ftell(texReader);  //| and output.
        printf("[%lu bytes] ", bytes);
        unsigned char* rawData = new unsigned char [bytes];
        fseek(texReader, 0L, SEEK_SET); 
        fread(rawData, sizeof(unsigned char), bytes, texReader);
        model.Textures.push_back(new SimpleTexture(rawData, bytes));
        fclose(texReader);
        printf("[OK]\n");
      }
      else printf("[ERROR]\n");
    }
    fread(&dbuf, sizeof(DWORD), 1, _fp);      // Read 'Texture Flags' (also not used).    
  }


  //_____________________________________________________________________________________[GEOSETS]
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


    // Scan for 'MATS' - Material association codeblock.
    while (dbuf != 'STAM') fread(&dbuf, sizeof(DWORD), 1, _fp);
    fread(&dbuf, sizeof(DWORD), 1, _fp);  // Read number of matrix indexes (we ignore them).
    for (i = dbuf; i > 0; i --) fread(&dbuf, sizeof(DWORD), 1, _fp); 
    fread(&dbuf, sizeof(DWORD), 1, _fp);  // This is our material ID!

    // Get associated texture from texture ID vector.
    if (dbuf < texIDs.size()) {
      if (texIDs[dbuf] < model.Textures.size()) geoset->textureID = texIDs[dbuf];
      else {
        printf("[ERROR] Texture association failed. Entry %d is invalid (only %d texture%s)!\n",
               texIDs[dbuf], model.Textures.size(), (model.Textures.size() > 1)? "s" : "");
        geoset->textureID = -1;      
      }
    }
    else {
      printf("[ERROR] Texture association failed. No texture entry for material ID %lu!\n", dbuf);
      geoset->textureID = -1;
    }


    // Skip to texture coordinates [UVBS].
    while (dbuf != 'SBVU') fread(&dbuf, sizeof(DWORD), 1, _fp);
    fread(&geoset->nrT, sizeof(DWORD), 1, _fp);
    totalT += geoset->nrT;
    geoset->texVects = new Float2[geoset->nrT];
    for (i = 0; i < geoset->nrT; i ++) {
      fread(&geoset->texVects[i].X, sizeof(float), 1, _fp);
      fread(&geoset->texVects[i].Y, sizeof(float), 1, _fp);
    }

    // Add new geoset.
    model.Geosets.push_back(geoset);
    idCounter ++;
  }


  //_____________________________________________________________________________________[BONES]
  // Scan for 'BONE' - animation bones codeblock.
  while (dbuf != 'ENOB') fread(&dbuf, sizeof(DWORD), 1, _fp);
  fread(&chunkSize, sizeof(DWORD), 1, _fp);   // Read total bone chunk size.

  // Read the bone chunk.
  for (unsigned int i = 0; i < chunkSize; i += inclSize) { 
    Bone* bone = new Bone();
    fread(&inclSize,       sizeof(DWORD), 1, _fp); // Read node structure total size.  
    fread(&bone->Name,     sizeof(char), 80, _fp); // Read name of bone.
    fread(&bone->ID,       sizeof(DWORD), 1, _fp); // Read 'ObjectID'.
    fread(&bone->ParentID, sizeof(DWORD), 1, _fp); // Read 'ParentID'.
    fread(&dbuf,           sizeof(DWORD), 1, _fp); // Skip 'Flags' (always 256).
 
    // Calculate remaining bytes of current subset.
    long remaining = inclSize - 4 * sizeof(DWORD) - 80 * sizeof(char);  
    
    // If available, read geoset transformation, rotation and scaling information.
    while (remaining > 0) {
      AnimSet* animSet = new AnimSet();
      fread(&dbuf,                   sizeof(DWORD), 1, _fp); // Read block identifier.
      fread(&animSet->Size,          sizeof(DWORD), 1, _fp); // Read element size.
      fread(&animSet->Interpolation, sizeof(DWORD), 1, _fp); // Read interpolation type.
      fread(&animSet->GlobalSeqID,   sizeof(DWORD), 1, _fp); // Read global sequence ID assignment.
      remaining -= 4 * sizeof(DWORD);

      animSet->Time = new long [animSet->Size];     // Allocate array for time progress.
      animSet->Values = new Float4 [animSet->Size]; // Allocate values array.
      for (int t = 0; t < animSet->Size; t ++) {
        fread(&animSet->Time[t], sizeof(DWORD), 1, _fp);    // Read time slot.
        fread(&animSet->Values[t], sizeof(Float3), 1, _fp); // Read 3D float value.
        remaining -= sizeof(DWORD) + sizeof(Float3);

        if (dbuf == 'TRGK') {
          fread(&animSet->Values[t].W, sizeof(float), 1, _fp); // Read 4th value (bank angle).
          remaining -= sizeof(float);
        }

        // Read [optional] tan value for Hermite and Bezier interpolation.
        if (animSet->Interpolation > 1) {
          animSet->InTan = new Float4();
          animSet->OutTan = new Float4();
          if (dbuf == 'TRGK') {
            fread(&animSet->InTan, sizeof(Float4), 1, _fp);
            fread(&animSet->OutTan, sizeof(Float4), 1, _fp);
            remaining -= 2 * sizeof(Float4);
          }
          else {
            fread(&animSet->InTan, sizeof(Float3), 1, _fp);
            fread(&animSet->OutTan, sizeof(Float3), 1, _fp);
            remaining -= 2 * sizeof(Float3);
          }
        }
      }

      // Write the constructed animation set to the corresponding pointer. 
      switch (dbuf) {       
        case 'RTGK': bone->Translation = animSet;  break; // 'KGTR' - Geoset translation block.   
        case 'TRGK': bone->Rotation    = animSet;  break; // 'KGRT' - Geoset rotation block.
        case 'CSGK': bone->Scaling     = animSet;  break; // 'KGSC' - Geoset scaling block.
      }
    }


    fread(&dbuf, sizeof(DWORD), 1, _fp);   // Read 'GeosetID'.
    fread(&dbuf, sizeof(DWORD), 1, _fp);   // Read 'GeosetAnimationID'.
    inclSize += 2*sizeof(DWORD);           // Increase read size by these last two fields.
    model.Bones.push_back(bone);
  }

  
  /* // DEBUG OUTPUT - KEEP UNTIL ALL WORKS PROPERLY
  for (unsigned int i = 0; i < bones.size(); i ++) {
    printf("Bone [%d]: %s (%d - %d) T: %d | R: %d | S: %d \n", 
           i, bones[i]->Name, bones[i]->ID, bones[i]->ParentID,
           (bones[i]->Translation == 0)? 0 : bones[i]->Translation->Size, 
           (bones[i]->Rotation == 0)? 0 : bones[i]->Rotation->Size, 
           (bones[i]->Scaling == 0)? 0 : bones[i]->Scaling->Size);
  }
  */
  

  // Print parser results.
  printf("Parser results:  \n"
         " - Geosets   : %d\n"
         " - Textures  : %d\n"
         " - Sequences : %d\n"
         " - Bones     : %d\n"
         " - Vertices  : %d\n"
         " - Normals   : %d\n"
         " - TexCoords : %d\n"
         " - Geometries: %d\n", 
           model.Geosets.size(), model.Textures.size(), model.Sequences.size(),
           model.Bones.size(), totalV, totalN, totalT, totalG);

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
      geoset->texVects = textures;
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
