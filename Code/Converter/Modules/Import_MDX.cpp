#include <Converter/Converter.h>
#include <stdlib.h>
#include <vector>
using namespace std;


struct MDX_Sequence {
  char Name[80];
  long IntervalStart;
  long IntervalEnd;
  bool Loop;
  float MoveSpeed;
  float BoundsRadius;
  Float3 MinimumExtent;
  Float3 MaximumExtent;
};

struct MDX_AnimSet {
  int Size;             // Number of values (array sizes). 
  short Interpolation;  // Interpolation type (0: none, 1: linear, 2: Hermite, 3: Bezier).
  int GlobalSeqID;      // Global sequence ID.
  long* Time;           // Timestep in animation.
  Float4* Values;       // Value: 3D vector (with bank angle on 4th for rotation spec [= quaternion]). 
  Float4* InTan;        //| Angles used for Hermite and 
  Float4* OutTan;       //| Bezier interpolation.
};

struct MDX_Bone {
  char Name[80];
  int ID;
  int ParentID;
  int GeosetID;
  Float3 Position;
  MDX_AnimSet* Translation = 0;
  MDX_AnimSet* Rotation = 0;
  MDX_AnimSet* Scaling = 0;
};



/** Reads a Blizzard MDX file (WarCraft 3).
 * @param inputfile The file to load.
 * @return The loaded model. No need for index alignment here! */
Model2* Converter::ReadMdx(const char* inputfile) {
  printf("MDX converter loaded.\n");
  FILE* fp = FileUtils::File_open(inputfile, true);
  if (fp == NULL) return NULL;

  // Create reader temporary variables.
  DWORD dbuf = 0x00;    // "Read-in and discard" buffer.
  DWORD remaining;      // Geoset byte size.
  DWORD chunkSize;      // Work storage for various chunk size read-ins.
  float fbuf;
  int totalV = 0, totalN = 0, totalT = 0, totalG = 0;
  
  // Create model and vectors for the reader intermediate results.
  Model2* model = new Model2();
  vector<MDX_Sequence> sequences;
  vector<MDX_Bone> bones;
  vector<DWORD> texIDs;
  vector<char*> textures;

  // Set the model name and version.
  char dir[256], file[80], name[40], ending[10];
  FileUtils::File_splitPath(inputfile, dir, file);
  FileUtils::File_splitEnding(file, name, ending);
  strcpy(model->Name, name);
  model->Version = 2;


  //________________________________________________________________________________[SEQUENCES]
  // Scan for 'SEQS' - animation sequence codeblock.
  while (dbuf != 'SQES') fread(&dbuf, sizeof(DWORD), 1, fp);
  fread(&chunkSize, sizeof(DWORD), 1, fp);   // Read total sequence chunk size.
  for (uint i = 0; i < chunkSize / 132; i ++) {
    MDX_Sequence seq = MDX_Sequence();
    fread(&seq.Name,          sizeof(char),  80, fp); // Read sequence name.
    fread(&seq.IntervalStart, sizeof(DWORD),  1, fp); // Read first animation frame.
    fread(&seq.IntervalEnd,   sizeof(DWORD),  1, fp); // Read last animation frame.
    fread(&seq.MoveSpeed,     sizeof(float),  1, fp); // Read movement speed.
    fread(&dbuf,              sizeof(DWORD),  1, fp); // Read flags (evaluation later). 
    if (dbuf == 0) seq.Loop = true;                   // Flags: 0: Looping, 1: NonLooping.                 
    fread(&fbuf,              sizeof(float),  1, fp); // Read 'Rarity' (whatever it is).
    fread(&dbuf,              sizeof(DWORD),  1, fp); // Read 'SyncPoint' (probably n.y.i). 
    fread(&seq.BoundsRadius,  sizeof(float),  1, fp); // Read bounds radius (broad phase CD).
    fread(&seq.MinimumExtent, sizeof(Float3), 1, fp); // Read minimum animation extent.
    fread(&seq.MaximumExtent, sizeof(Float3), 1, fp); // Read maximum animation extent.
    sequences.push_back(seq);
  }



  //________________________________________________________________________________[MATERIALS]
  // Scan for 'MTLS' - materials codeblock.
  while (dbuf != 'SLTM') fread(&dbuf, sizeof(DWORD), 1, fp);
  fread(&chunkSize, sizeof(DWORD), 1, fp);    // Read total material chunk size.
  DWORD inclSize;                             // Material inclusive size storage.

  // Read all materials.
  for (uint i = 0; i < chunkSize; i += inclSize) {
    fread(&inclSize, sizeof(DWORD), 1, fp);  // Read 'InclusiveSize'.
    fread(&dbuf, sizeof(DWORD), 1, fp);      // 'Priority Plane'.
    fread(&dbuf, sizeof(DWORD), 1, fp);      // 'Flags'.      
    fread(&dbuf, sizeof(DWORD), 1, fp);      // Skip the 'LAYS' keyword.
    DWORD nrLayers;                          // Number of layers following.
    fread(&nrLayers, sizeof(DWORD), 1, fp);  // Read number of layers.

    // Read all layers of current material.
    for (unsigned int l = 0; l < nrLayers; l ++) {
      DWORD lInclSize;
      fread(&lInclSize, sizeof(DWORD), 1, fp); // Read layer inclusive size.
      fread(&dbuf, sizeof(DWORD), 1, fp);      // 'FilterMode'.
      fread(&dbuf, sizeof(DWORD), 1, fp);      // 'ShadingFlags'.
      fread(&dbuf, sizeof(DWORD), 1, fp);      // 'TextureID'.
      texIDs.push_back(dbuf);                  // Save the texture ID for later association.

      // Because we don't need all the following stuff at the moment, we just skip it.
      long remaining = lInclSize - 4*sizeof(DWORD);
      fseek(fp, remaining, SEEK_CUR);
      //fread(&dbuf, sizeof(DWORD), 1, fp);      // 'TextureAnimationID'.
      //fread(&dbuf, sizeof(DWORD), 1, fp);      // 'CoordID'.
      //fread(&dbuf, sizeof(float), 1, fp);      // 'Alpha'.
      // Read material alpha structure.
      // Read material texture ID structure.    
    }
  }



  //_________________________________________________________________________________[TEXTURES]
  // Scan for 'TEXS' - textures codeblock.
  while (dbuf != 'SXET') fread(&dbuf, sizeof(DWORD), 1, fp);
  fread(&chunkSize, sizeof(DWORD), 1, fp);

  // Iterate over all texture paths.
  for (uint t = 0; t < chunkSize / 268; t ++) {
    char* texPath = (char*) calloc(260, sizeof(char));    
    fread(&dbuf, sizeof(DWORD), 1, fp);      // Read (and discard) 'Replaceable ID'.    
    fread(texPath, sizeof(char), 260, fp);   // Read texture path.
    fread(&dbuf, sizeof(DWORD), 1, fp);      // Read 'Texture Flags' (also not used).    
    textures.push_back(texPath);
  }

  
  /* Reading the texture files. */
  vector<short> texIdx;
  for (uint t = 0; t < textures.size(); t ++) {
    printf("Loading texture '%s' ", textures[t]);

    // Try to open file stream.
    FILE* texReader = fopen(textures[t], "rb");
    if (texReader != NULL) {  
      fseek(texReader, 0L, SEEK_END);          //| Read image size
      unsigned long bytes = ftell(texReader);  //| and output.
      printf("[%lu bytes] ", bytes);
      BYTE* rawData = (BYTE*) calloc(bytes, sizeof(BYTE));
      fseek(texReader, 0L, SEEK_SET);
      fread(rawData, sizeof(BYTE), bytes, texReader);
      model->Textures.push_back(new SimpleTexture(rawData, bytes, textures[t]));
      fclose(texReader);
      printf("[OK]\n");
      texIdx.push_back(t);
    }
    else {
      printf("[ERROR]\n");
      texIdx.push_back(-1);
    }
  }




  //__________________________________________________________________________________[GEOSETS]
  // Skip file to geoset definitions and read in size.
  while (dbuf != 'SOEG') fread(&dbuf, sizeof(DWORD), 1, fp);
  fread(&remaining, sizeof(DWORD), 1, fp);
  for (uint g = 0; remaining > 0; g ++) {
    
    // Read size of this geoset and reduce remaining size.
    fread(&dbuf, sizeof(DWORD), 1, fp);
    remaining -= dbuf;       

    // Setup mesh and submesh structure for this geoset.
    Mesh2 mesh = Mesh2();
    sprintf(mesh.ID, "MDX-Geoset %02d", g);
    mesh.IndexOffset = totalG;
    mesh.Enabled = true;  //TODO To be set later by some function.


    // Read all vertices.
    uint nrVertices;
    fread(&dbuf, sizeof(DWORD), 1, fp); // Skip 'VRTX'.
    fread(&nrVertices, sizeof(DWORD), 1, fp);
    totalV += nrVertices;
    model->Vertices.reserve(totalV);
    for (uint vtx = 0; vtx < nrVertices; vtx ++) {
      Float3 v = Float3();
      fread(&v.X, sizeof(float), 1, fp);
      fread(&v.Y, sizeof(float), 1, fp);
      fread(&v.Z, sizeof(float), 1, fp);
      model->Vertices.push_back(v);
    }

    // Read the normals.
    uint nrNormals;
    fread(&dbuf, sizeof(DWORD), 1, fp); // Skip 'NRMS'.
    fread(&nrNormals, sizeof(DWORD), 1, fp);
    totalN += nrNormals;
    model->Normals.reserve(totalN);
    for (uint nor = 0; nor < nrNormals; nor ++) {
      Float3 n = Float3();
      fread(&n.X, sizeof(float), 1, fp);
      fread(&n.Y, sizeof(float), 1, fp);
      fread(&n.Z, sizeof(float), 1, fp);
      model->Normals.push_back(n);
    }
    
    // Scan to faces definition.
    while (dbuf != 'XTVP') fread(&dbuf, sizeof(DWORD), 1, fp);  // Skip 'PVTX'.
    uint nrIndices;
    fread(&nrIndices, sizeof(DWORD), 1, fp);
    totalG += nrIndices;
    model->Indices.reserve(totalG);
    WORD index;
    for (uint idx = 0; idx < nrIndices; idx ++) {
      fread(&index, sizeof(WORD), 1, fp);
      index += (totalV-nrVertices);  // Read indices are relative to the geoset. Add offset! 
      model->Indices.push_back(index);
    }
    fread(&dbuf, sizeof(DWORD), 1, fp);  // Skip 'GNDX'.
    fread(&dbuf, sizeof(DWORD), 1, fp);  // Discard vertex group numbers.
    for (DWORD i = dbuf; i > 0; i --) fread(&dbuf, sizeof(BYTE), 1, fp);

    
    // Scan for 'MATS' - Material association codeblock.
    while (dbuf != 'STAM') fread(&dbuf, sizeof(DWORD), 1, fp);
    fread(&dbuf, sizeof(DWORD), 1, fp);  // Read number of matrix indexes (we ignore them).
    for (DWORD i = dbuf; i > 0; i --) fread(&dbuf, sizeof(DWORD), 1, fp);
    fread(&dbuf, sizeof(DWORD), 1, fp);  // This is our material ID!

    // Get associated texture from texture ID vector.
    if (dbuf < texIDs.size()) {
      if (texIDs[dbuf] < textures.size()) {
        strcpy(mesh.Texture, textures[texIDs[dbuf]]);
        mesh.TextureIdx = texIdx[texIDs[dbuf]];
      }
      else {
        printf("[ERROR] Texture association failed. Entry %d is invalid (only %d texture%s)!\n",
          texIDs[dbuf], textures.size(), (textures.size() > 1)? "s" : "");
        strcpy(mesh.Texture, "");
        mesh.TextureIdx = -1;
      }
    }
    else { // Texture index is out of bounds. Print error and leave texture field empty. 
      printf("[ERROR] Texture association failed. No texture entry for material ID %lu!\n",dbuf);
      strcpy(mesh.Texture, "");
    }


    // Skip to texture coordinates [UVBS].
    while (dbuf != 'SBVU') fread(&dbuf, sizeof(DWORD), 1, fp);
    uint nrUVs;
    fread(&nrUVs, sizeof(DWORD), 1, fp);
    totalT += nrUVs;
    model->UVs.reserve(nrUVs);
    for (uint uv = 0; uv < nrUVs; uv ++) {
      Float2 tex = Float2();
      fread(&tex.X, sizeof(float), 1, fp);
      fread(&tex.Y, sizeof(float), 1, fp);
      model->UVs.push_back(tex);
    }

    // Add new mesh.
    mesh.IndexLength = nrIndices;
    mesh.BoneCount = 0;
    mesh.BoneOffset = 0;
    model->Meshes.push_back(mesh);
  }



  //____________________________________________________________________________________[BONES]
  // Scan for 'BONE' - animation bones codeblock.
  while (dbuf != 'ENOB') fread(&dbuf, sizeof(DWORD), 1, fp);
  fread(&chunkSize, sizeof(DWORD), 1, fp);   // Read total bone chunk size.

  // Read the bone chunk.
  for (uint i = 0; i < chunkSize; i += inclSize) {
    MDX_Bone bone = MDX_Bone();
    fread(&inclSize,      sizeof(DWORD), 1, fp); // Read node structure total size.  
    fread(&bone.Name,     sizeof(char), 80, fp); // Read name of bone.
    fread(&bone.ID,       sizeof(DWORD), 1, fp); // Read 'ObjectID'.
    fread(&bone.ParentID, sizeof(DWORD), 1, fp); // Read 'ParentID'.
    fread(&dbuf,          sizeof(DWORD), 1, fp); // Skip 'Flags' (always 256).

    // Calculate remaining bytes of current subset.
    long remaining = inclSize - 4 * sizeof(DWORD) - 80 * sizeof(char);

    // If available, read geoset transformation, rotation and scaling information.
    while (remaining > 0) {
      MDX_AnimSet* animSet = new MDX_AnimSet();
      fread(&dbuf,                   sizeof(DWORD), 1, fp); // Read block identifier.
      fread(&animSet->Size,          sizeof(DWORD), 1, fp); // Read element size.
      fread(&animSet->Interpolation, sizeof(DWORD), 1, fp); // Read interpolation type.
      fread(&animSet->GlobalSeqID,   sizeof(DWORD), 1, fp); // Read global sequence assignment.
      remaining -= 4 * sizeof(DWORD);

      animSet->Time = new long[animSet->Size];     // Allocate array for time progress.
      animSet->Values = new Float4[animSet->Size]; // Allocate values array.
      for (int t = 0; t < animSet->Size; t ++) {
        fread(&animSet->Time[t],    sizeof(DWORD), 1, fp);  // Read time slot.
        fread(&animSet->Values[t], sizeof(Float3), 1, fp);  // Read 3D float value.
        remaining -= sizeof(DWORD) + sizeof(Float3);

        if (dbuf == 'TRGK') {
          fread(&animSet->Values[t].W, sizeof(float), 1, fp); // Read 4th value (bank angle).
          remaining -= sizeof(float);
        }

        // Read [optional] tan value for Hermite and Bezier interpolation.
        if (animSet->Interpolation > 1) {
          animSet->InTan = new Float4();
          animSet->OutTan = new Float4();
          if (dbuf == 'TRGK') {
            fread(&animSet->InTan,  sizeof(Float4), 1, fp);
            fread(&animSet->OutTan, sizeof(Float4), 1, fp);
            remaining -= 2 * sizeof(Float4);
          }
          else {
            fread(&animSet->InTan,  sizeof(Float3), 1, fp);
            fread(&animSet->OutTan, sizeof(Float3), 1, fp);
            remaining -= 2 * sizeof(Float3);
          }
        }
      }

      // Write the constructed animation set to the corresponding pointer. 
      switch (dbuf) {
        case 'RTGK': bone.Translation = animSet;  break; // 'KGTR' - Geoset translation block.   
        case 'TRGK': bone.Rotation = animSet;     break; // 'KGRT' - Geoset rotation block.
        case 'CSGK': bone.Scaling = animSet;      break; // 'KGSC' - Geoset scaling block.
      }
    }

    fread(&dbuf, sizeof(DWORD), 1, fp);   // Read 'GeosetID'.
    fread(&dbuf, sizeof(DWORD), 1, fp);   // Read 'GeosetAnimationID'.
    inclSize += 2 * sizeof(DWORD);        // Increase read size by these last two fields.
    bones.push_back(bone);
  }



  /* READING THE PIVOT POINTS. Probably there are more for lights, particles etc... */
  while (dbuf != 'TVIP') fread(&dbuf, sizeof(DWORD), 1, fp);
  fread(&chunkSize, sizeof(DWORD), 1, fp);   // Read pivot chunk size.
  for (uint i = 0, b = 0; i < chunkSize / 12; i ++) {
    if (b < bones.size()) {
      fread(&bones[b].Position, sizeof(Float3), 1, fp);
      b ++;
    }
  }



  // DEBUG OUTPUT - KEEP UNTIL ALL WORKS PROPERLY
  for (uint i = 0; i < bones.size(); i ++) {
    printf("Bone [%d]: %s (ID: %d - Par: %d) T: %d | R: %d | S: %d  /  Pos: %f, %f, %f\n",
      i, bones[i].Name, bones[i].ID, bones[i].ParentID,
      (bones[i].Translation == 0)? 0 : bones[i].Translation->Size,
      (bones[i].Rotation    == 0)? 0 : bones[i].Rotation->Size,
      (bones[i].Scaling     == 0)? 0 : bones[i].Scaling->Size,
      bones[i].Position.X, bones[i].Position.Y, bones[i].Position.Z);
  }

  /* 
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
*/


  // Print parser results.
  printf("Parser results:\n"
    " - Meshes  : %d\n"
    " - Vertices: %d\n"
    " - Normals : %d\n"
    " - UVs     : %d\n"
    " - Indices : %d\n",
    model->Meshes.size(), model->Vertices.size(), model->Normals.size(), 
    model->UVs.size(), model->Indices.size());
  
  fclose(fp); // Close file stream.
  return model;
} 
