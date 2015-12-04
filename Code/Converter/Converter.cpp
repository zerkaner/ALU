#pragma warning(disable: 4996)
#include <Converter/Converter.h>
#include <Converter/Converter2.h>
#include <Data/Model3D.h>
#include <Data/Primitives.h>
#include <Data/Textures/Texture.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>


Converter::Converter(int argc, char** argv) {
  
  // Scaling argument supplied. Read it first, then start conversion.
  if (argc == 3 && (strstr(argv[0], "--scale=") != NULL)) {
    ParseScalingParam(argv[0]);
    _savename = argv[2];
    StartConversion(argv[1]);   
  }

  // Only source and target specified.  
  else if (argc == 2) {
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
  //TODO printf("\nOpening file '%s' [%lu bytes].\n", filename, bytes);


  // Execute corresponding converter function.
  switch (fileformat) {
    case GLF: ReadGLF();  break;
    case OBJ: /*ReadOBJ();*/ Converter2().ConvertObj(filename, true); return; //TODO ← Hier eingehängt!!
    case MDX: /*ReadMDX();*/ Converter2().ConvertObj(filename, false); return;
    default: printf("Sorry, converter module is not implemented yet!\n");
  }

  // Close input file stream after conversion.
  fclose(_fp);

  // Model conversion successful. Apply transformatons, then write it to disk.
  if (_model != NULL) {

    // Look out for scaling parameter.
    if (_scaleAxis != ' ' && _scaleVal > 0) _model->ScaleModelToExtent(_scaleAxis, _scaleVal);
    else if (_scaleVal > 0) _model->ScaleModel(_scaleVal);

    if (strstr(_savename, ".json") != NULL) SaveModelAsJson(); // Save in JSON format.   
	  else _model->WriteFile(_savename);                         // Default save as M4.
	  delete _model;
  }
}



/** Utility method to check the beginning of a string. [internal] 
 * @param string The string to check.
 * @param prefix The beginning to search for.
 * @return '1', if true, '0' otherwise. */
int StartsWith (const char* string, const char* prefix) {
  size_t lenpre = strlen (prefix), lenstr = strlen (string);
  return (lenstr < lenpre)? 0 : (strncmp (prefix, string, lenpre) == 0);
}



void Converter::ParseScalingParam(char arg[]) {
  char* splitPtr = strtok(arg, "=");  
  splitPtr = strtok(NULL, "=");         // Now we have everything after "scale=".    
  if (strstr(splitPtr, ":") != NULL) {  
    splitPtr = strtok(splitPtr, ":");   //| If an axis is supplied, we now have it
    _scaleAxis = splitPtr[0];           //| (it is before ':') and advance once more.    
    splitPtr = strtok(NULL, ":");       
  }                                 
  else splitPtr = strtok(splitPtr, ":"); // Otherwise we already are at the value.  
  sscanf(splitPtr, "%f", &_scaleVal);
}



void Converter::SaveModelAsJson() {

  // Try to open writer stream. Quit on failure.
  FILE* writer = fopen(_savename, "w");
  if (writer == NULL) {
    printf("\nError writing to file '%s'!\n", _savename);
    return;
  }

  // Check for asymmetric indices. Yes, the flag definitely belongs somewhere else ...
  bool aligned = true;
  for (unsigned int g = 0; g < _model->Geosets.size(); g++) {
    for (int t = 0; t < _model->Geosets[g]->nrG; t++) {
      if (!_model->Geosets[g]->geometries[t].symIndices) {
        aligned = false;
        break;
      }
    }
  }
  if (!aligned) AlignIndices();

  // Split file name from the full save path.
  char temp[20];
  strcpy(temp, _savename);
  char* modelname = strtok((char*)temp, "\\/");
  char* probePtr;
  while ((probePtr = strtok(NULL, "\\/")) != NULL) modelname = probePtr;

  // Write JSON file head.
  fprintf(writer, "{\n");
  fprintf(writer, "  \"modelname\": \"%s\",\n", modelname);
  fprintf(writer, "  \"geosets\": [\n");
  for (unsigned int g = 0; g < _model->Geosets.size(); g++) {
    Geoset* geoset = _model->Geosets[g];

    // Write geoset ID and texture path (if texture exists).
    fprintf(writer, "    {\n");
    fprintf(writer, "      \"id\"       : \"geoset-%d\",\n", g);
    fprintf(writer, "      \"texture\"  : \"%s\",\n",
      (geoset->textureID != -1)? _model->Textures[geoset->textureID]->Name() : "");
    
    // Vertex output.
    fprintf(writer, "      \"vertices\" : [");
    for (int i = 0; i < geoset->nrV; i++) {
      Float3 vertex = geoset->vertices[i];
      fprintf(writer, "%f,%f,%f%s", vertex.X, vertex.Y, vertex.Z, (i < geoset->nrV - 1) ? "," : "");
    }

    // Normal vector output.
    fprintf(writer, "],\n      \"normals\"  : [");
    for (int i = 0; i < geoset->nrN; i++) {
      Float3 normal = geoset->normals[i];
      fprintf(writer, "%f,%f,%f%s", normal.X, normal.Y, normal.Z, (i < geoset->nrN - 1) ? "," : "");
    }
    
    // Texture coordinates output.
    fprintf(writer, "],\n      \"texcoords\": [");
    for (int i = 0; i < geoset->nrT; i++) {
      Float2 texCoord = geoset->texVects[i];
      fprintf(writer, "%f,%f%s", texCoord.X, texCoord.Y, (i < geoset->nrT - 1) ? "," : "");
    }

    // Triangle indices output.
    fprintf(writer, "],\n      \"indices\"  : [");
    for (int i = 0; i < geoset->nrG; i++) {
      Geometry tri = geoset->geometries[i];
      fprintf(writer, "%d,%d,%d%s", tri.vIdx[0], tri.vIdx[1], tri.vIdx[2],(i < geoset->nrG - 1) ? "," : "");
    }

    fprintf(writer, "]\n    }%s\n", (g < _model->Geosets.size()-1)? "," : "");
  }
  fprintf(writer, "  ]\n}");


  // Close output file stream.
  printf("Written %d bytes to file '%s'.\n", ftell(writer), _savename);
  fclose(writer);
}



/** The following two structures belong to the AlignIndices() function 
 *  but have to be declared outside to be GCC (C++ 2003) compliant. **/

/** Intermediate structure to associate position-, normal- and texture vector. */
struct Vertex { Float3 position; Float3 normal; Float2 texel; };

/** Indexing structure; corresponds to a face definition. */
struct VertexIndex {
  int v, vn, vt;
  bool operator<(const VertexIndex& other) const {
    char s1[20]; char s2[20];
    sprintf(s1, "%d-%d-%d", v, vn, vt);
    sprintf(s2, "%d-%d-%d", other.v, other.vn, other.vt);
    return (strcmp(s1, s2) < 0);
  }
};



void Converter::AlignIndices() {
  printf("Aligning indices ... ");

  // Loop over all geosets.
  for (unsigned int g = 0; g < _model->Geosets.size(); g++) {
    Float3* vertices = _model->Geosets[g]->vertices;       //|
    Float3* normals = _model->Geosets[g]->normals;         //| Just shortcuts 
    Float2* texels = _model->Geosets[g]->texVects;         //| for easier access!
    Geometry* triangles = _model->Geosets[g]->geometries;  //|

    std::map<VertexIndex, int> indexMapping;  // Triangle indices to new (uniform) index mapping.
    std::vector<Vertex> vertexBuffer;         // Buffer for rearranged vertices (interleaved v/n/t).
    std::vector<int> indexBuffer;             // Buffer for the new indices.

    for (int t = 0; t < _model->Geosets[g]->nrG; t++) { // Loops over all triangles.  
      for (int i = 0; i < 3; i++) {                     // Loops over the three vertices of a triangle.

        VertexIndex indices = VertexIndex();
        indices.v  = triangles[t].vIdx[i];
        indices.vn = triangles[t].nIdx[i];
        indices.vt = triangles[t].tIdx[i];   

        int index = -1;

        // If this <v,vn,vt> pair already has an index, we use it!
        if (indexMapping.count(indices)) index = indexMapping[indices];
        
        else {  // Otherwise we append a new vertex and index.    
          index = vertexBuffer.size();
          Vertex vtx = Vertex();
          vtx.position = vertices[indices.v];
          vtx.normal = normals[indices.vn];
          vtx.texel = texels[indices.vt];
          vertexBuffer.push_back(vtx);
          indexMapping[indices] = index;
        }
        indexBuffer.push_back(index);
      }
    }

    // Set new array and element sizes.
    _model->Geosets[g]->nrV = vertexBuffer.size();
    _model->Geosets[g]->nrN = vertexBuffer.size();
    _model->Geosets[g]->nrT = vertexBuffer.size();
    delete[] vertices;   vertices = new Float3[vertexBuffer.size()];
    delete[] normals;     normals = new Float3[vertexBuffer.size()];
    delete[] texels;       texels = new Float2[vertexBuffer.size()];
    
    for (unsigned int i = 0; i < vertexBuffer.size(); i++) {
      vertices[i] = vertexBuffer[i].position;
      normals[i]  = vertexBuffer[i].normal;
      texels[i]   = vertexBuffer[i].texel;
    }
    for (int i = 0; i <_model->Geosets[g]->nrG; i ++) {
      triangles[i].symIndices = true;
      triangles[i].vIdx[0] = triangles[i].nIdx[0] = triangles[i].tIdx[0] = indexBuffer[(i*3)];
      triangles[i].vIdx[1] = triangles[i].nIdx[1] = triangles[i].tIdx[1] = indexBuffer[(i*3)+1];
      triangles[i].vIdx[2] = triangles[i].nIdx[2] = triangles[i].tIdx[2] = indexBuffer[(i*3)+2];
    }
    
    // Assign the new arrays to the original geoset pointer.
    _model->Geosets[g]->vertices = vertices;
    _model->Geosets[g]->normals = normals;
    _model->Geosets[g]->texVects = texels;
  }  
  printf("[finished]\n");
}



void Converter::ReadMDX() {
  printf("MDX converter loaded.\n");
 
  // Create variables and 3D model.
  DWORD dbuf = 0x00;          // "Read-in and discard" buffer.
  DWORD remaining;            // Geoset byte size.
  DWORD chunkSize;            // Work storage for various chunk size read-ins.
  float fbuf;
  int i;
  int totalV = 0, totalN = 0, totalT = 0, totalG = 0;
  int idCounter = 0;
  _model = new Model3D();


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
    fread(&seq->MinimumExtent, sizeof(Float3), 1, _fp); // Read minimum animation extent.
    fread(&seq->MaximumExtent, sizeof(Float3), 1, _fp); // Read maximum animation extent.
    _model->Sequences.push_back(seq);
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
    char* texPath = (char*) calloc(260, sizeof(char));
    fread(texPath, sizeof(char), 260, _fp);   // Read texture path.
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
        _model->Textures.push_back(new SimpleTexture(rawData, bytes, texPath));
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
      if (texIDs[dbuf] < _model->Textures.size()) geoset->textureID = texIDs[dbuf];
      else {
        printf("[ERROR] Texture association failed. Entry %d is invalid (only %d texture%s)!\n",
               texIDs[dbuf], _model->Textures.size(), (_model->Textures.size() > 1)? "s" : "");
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
    _model->Geosets.push_back(geoset);
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
    _model->Bones.push_back(bone);
  }

  /*
  // DEBUG OUTPUT - KEEP UNTIL ALL WORKS PROPERLY
  for (unsigned int i = 0; i < _model->Bones.size(); i ++) {
    printf("Bone [%d]: %s (%d - %d) T: %d | R: %d | S: %d \n", 
      i, _model->Bones[i]->Name, _model->Bones[i]->ID, _model->Bones[i]->ParentID,
      (_model->Bones[i]->Translation == 0) ? 0 : _model->Bones[i]->Translation->Size,
      (_model->Bones[i]->Rotation    == 0) ? 0 : _model->Bones[i]->Rotation->Size,
      (_model->Bones[i]->Scaling     == 0) ? 0 : _model->Bones[i]->Scaling->Size);
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
           _model->Geosets.size(), _model->Textures.size(), _model->Sequences.size(),
           _model->Bones.size(), totalV, totalN, totalT, totalG);


  printf("[DBG] Creating bone hierarchy:\n");
  

  // Some pointer wraps to keep code below context-independent!
  // Later, these global structures shall be replaced with local ones.
  std::vector<Bone*> bones = _model->Bones;
  std::vector<Sequence*> sequences = _model->Sequences;


  Bone_t* skeleton = (Bone_t*) calloc(bones.size(), sizeof(Bone_t));
  int nrBones = bones.size();

  // First run: Set base data and determine child node counters.
  for (int i = 0; i < nrBones; i ++) {
    skeleton[i].ID = bones[i]->ID;                         // Set the bone ID.
    skeleton[i].Name = bones[i]->Name;                     // Set the name.
    if (bones[i]->ParentID != -1) {                        // If a parent node exists ...
      skeleton[i].Parent = &skeleton[bones[i]->ParentID];  // - set link to parent node
      skeleton[bones[i]->ParentID].NrChildren ++;          // - and increase the parent's child count.
    }
    else skeleton[i].Parent = NULL;
  }

  // Second run: Allocate memory for all nodes with children.
  int* tmpSet = (int*) calloc(nrBones, sizeof(int));
  for (int i = 0; i < nrBones; i ++) {
    if (skeleton[i].NrChildren > 0) {
      skeleton[i].Children = (Bone_t**) calloc(skeleton[i].NrChildren, sizeof(Bone_t*));
    }
  }

  // Third run: Set the child nodes.
  for (int i = 0; i < nrBones; i ++) {
    if (skeleton[i].Parent != NULL) {
      skeleton[i].Parent->Children[tmpSet[bones[i]->ParentID]] = &skeleton[i];
      tmpSet[bones[i]->ParentID] ++;  // Increase array index (to write the next value).
    }
  }

  //TODO Vertex group assignments!
  //TODO Local sequentation!
  //TODO Animation frames!
  // <-----
  Animation* animations = (Animation*) calloc(sequences.size(), sizeof(Animation));
  int nrAnimations = sequences.size();
  for (int i = 0; i < nrAnimations; i ++) {
    animations[i].Name = sequences[i]->Name;
    animations[i].FrameCount = sequences[i]->IntervalEnd - sequences[i]->IntervalStart + 1;

    // Calculate the number of transformation directives per bone.
    for (int j = 0; j < nrBones; j ++) {
      int counter = 0;
      
      AnimSet* set = bones[j]->Translation;
      if (set != NULL) {
        for (int k = 0; k < set->Size; k ++) {
          if (set->Time[k] >= sequences[i]->IntervalStart && set->Time[k] <= sequences[i]->IntervalEnd) {
            counter ++;
          }
        }
      }





      printf("[%s][%s]: %d TDs found.\n", animations[i].Name, skeleton[j].Name, counter);
    }



    //sequences[i]->IntervalStart, sequences[i]->IntervalEnd);
  }



  // Debug output. Echo all nodes and childs to the console (later, a file).
  for (int i = 0; i < nrBones; i ++) {
    printf("Node %02d ('%s'), parent: '%s', children (%d)", 
      skeleton[i].ID, 
      skeleton[i].Name, 
      (skeleton[i].Parent != NULL)? skeleton[i].Parent->Name : "",
      skeleton[i].NrChildren
    );
    if (skeleton[i].NrChildren > 0) {
      printf(": '%s'", skeleton[i].Children[0]->Name);
      for (unsigned int j = 1; j < skeleton[i].NrChildren; j ++) {
        printf(", '%s'", skeleton[i].Children[j]->Name);
      }
    }
    printf("\n");
  }

  for (int i = 0; i < nrAnimations; i ++) {
    printf("Animation '%-22s': %5d frames.", animations[i].Name, animations[i].FrameCount);
    printf("\n");
  }
}



void Converter::ReadOBJ() {
  printf ("OBJ converter loaded.\n");

  int v, vn, vt;          // Index variables for faces match.
  char buffer [256];      // Input buffer for file read-in.
  char mtlfile [80];      // Material library file name.
  _model = new Model3D(); // Create model structure.

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
        if (sscanf(splitPtr, "%d/%d/%d", &v, &vt, &vn) == 3) {  // Vertex, texel and normal.
          geometries[curG].vIdx [count-1] = v  - foV;
          geometries[curG].nIdx [count-1] = vn - foN;
          geometries[curG].tIdx [count-1] = vt - foT;      
          //TODO What if there are no n or t vrtx?
        }
        else if (sscanf(splitPtr, "%d//%d", &v, &vn) == 2) {    // Vertex and normal.
          geometries[curG].vIdx[count-1] = v  - foV;
          geometries[curG].nIdx[count-1] = vn - foN;
          geometries[curG].tIdx[count-1] = 0;
        }        
        else if (sscanf(splitPtr, "%d/%d", &v, &vt) == 2) {     // Vertex and texel.
          geometries[curG].vIdx[count-1] = v  - foV;
          geometries[curG].nIdx[count-1] = 0;
          geometries[curG].tIdx[count-1] = vt - foT;
        }
        else if (sscanf(splitPtr, "%d", &v) == 1) {             // Vertex only.
          geometries[curG].vIdx[count-1] =  v - foV;
          geometries[curG].nIdx[count-1] =  0;
          geometries[curG].tIdx[count-1] =  0;
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
      geoset->textureID = -1;
      geoset->geometries = geometries;
      _model->Geosets.push_back(geoset);
      
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
          " - TexCoords : %d\n"
          " - Geometries: %d\n", 
            geos, tV, tN, materials.size(), tT, tG);
}



void Converter::ReadGLF() {
  printf("GLF reader engaged!\n");

  // Try to open writer stream. Quit on failure.
  FILE* writer = fopen(_savename, "w");
  if (writer == NULL) {
    printf ("\nError writing to file '%s'!\n", _savename);
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
  printf ("Written %d bytes to file '%s'.\n", ftell(writer), _savename);  
  fclose(writer);
}
