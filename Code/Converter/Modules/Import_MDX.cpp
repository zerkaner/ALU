#include <Converter/Converter.h>
#include <stdio.h>
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

struct MDX_BoneAssignment {
  vector<uint> Associations;
  vector<vector<BYTE> > MatrixGroups;
};


/** Forward-declaration of helper functions. */
static MDX_Bone ReadMDXNode(FILE* fp, DWORD& inclSize);
static MDX_BoneAssignment ReadMDXBoneAssignment(FILE* fp);
static void ReorderBones(Model* model);
static void BuildVertexGroups(Model* model, vector<MDX_BoneAssignment> ba);



/** Reads a Blizzard MDX file (WarCraft 3).
 * @param inputfile The file to load.
 * @return The loaded model. No need for index alignment here! */
Model* Converter::ReadMdx(const char* inputfile) {
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
  Model* model = new Model();
  vector<MDX_Sequence> sequences;
  vector<MDX_Bone> bones;
  vector<MDX_BoneAssignment> associations;
  vector<DWORD> texIDs;
  vector<char*> textures;

  // Set the model name and version.
  char dir[256], file[80], name[40], ending[10];
  FileUtils::File_splitPath(inputfile, dir, file);
  FileUtils::File_splitEnding(file, name, ending);
  strcpy(model->Name, name);
  model->Version = 2;


  //__________________________________________________________________________________[SEQUENCES]
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
    seq.Loop = (dbuf == 0);                           // Flags: 0: Looping, 1: NonLooping.                 
    fread(&fbuf,              sizeof(float),  1, fp); // Read 'Rarity' (whatever it is).
    fread(&dbuf,              sizeof(DWORD),  1, fp); // Read 'SyncPoint' (probably n.y.i). 
    fread(&seq.BoundsRadius,  sizeof(float),  1, fp); // Read bounds radius (broad phase CD).
    fread(&seq.MinimumExtent, sizeof(Float3), 1, fp); // Read minimum animation extent.
    fread(&seq.MaximumExtent, sizeof(Float3), 1, fp); // Read maximum animation extent.
    sequences.push_back(seq);
  }



  //__________________________________________________________________________________[MATERIALS]
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



  //___________________________________________________________________________________[TEXTURES]
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




  //____________________________________________________________________________________[GEOSETS]
  // Skip file to geoset definitions and read in size.
  while (dbuf != 'SOEG') fread(&dbuf, sizeof(DWORD), 1, fp);
  fread(&remaining, sizeof(DWORD), 1, fp);
  for (uint g = 0; remaining > 0; g ++) {
    
    // Read size of this geoset and reduce remaining size.
    fread(&dbuf, sizeof(DWORD), 1, fp);
    remaining -= dbuf;       

    // Setup mesh and submesh structure for this geoset.
    Mesh mesh = Mesh();
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


    associations.push_back(ReadMDXBoneAssignment(fp));


    // Read the material ID.
    fread(&dbuf, sizeof(DWORD), 1, fp);

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
    model->Meshes.push_back(mesh);
  }


  bool repeatLoop = true;
  while (repeatLoop) {
    fread(&dbuf, sizeof(DWORD), 1, fp); // Read identifier flag.
    switch (dbuf) {

      //__________________________________________________________________________________[BONES]
      // Scan for 'BONE' - animation bones codeblock.
      case 'ENOB': {
        fread(&chunkSize, sizeof(DWORD), 1, fp);   // Read total bone chunk size.
        for (uint i = 0; i < chunkSize; i += inclSize) {
          MDX_Bone bone = ReadMDXNode(fp, inclSize);
          fread(&dbuf, sizeof(DWORD), 1, fp);   // Read 'GeosetID'.
          fread(&dbuf, sizeof(DWORD), 1, fp);   // Read 'GeosetAnimationID'.
          inclSize += 2 * sizeof(DWORD);        // Increase read size by these last two fields.
          bones.push_back(bone);
        }
        break;
      }

      //________________________________________________________________________________[HELPERS]
      // Scan for 'HELP' - auxiliary nodes ... and the main bone on some models (?!)
      case 'PLEH': {
        fread(&chunkSize, sizeof(DWORD), 1, fp);         // Read helper chunk size.
        for (uint i = 0; i < chunkSize; i += inclSize) { // Read the auxiliary nodes.  
          MDX_Bone bone = ReadMDXNode(fp, inclSize);
          bones.push_back(bone);
        }
        break;
      }

      //_________________________________________________________________________________[PIVOTS]
      // Reading the pivot points: These contain among others the bone positions.
      case 'TVIP': { 
        fread(&chunkSize, sizeof(DWORD), 1, fp);   // Read pivot chunk size.
        for (uint i = 0, b = 0; i < chunkSize / 12; i ++) {
          if (b < bones.size()) {
            fread(&bones[b].Position, sizeof(Float3), 1, fp);
            b ++;
          }
        }  
        repeatLoop = false; // Also break the while loop, because this is the last block.
        break;
      }
    }
  }

  // Write the bones to the model.
  for (uint i = 0; i < bones.size(); i ++) {
    Bone b = Bone();
    strcpy(b.Name, bones[i].Name);
    b.Parent = bones[i].ParentID;
    b.Pivot = bones[i].Position;
    model->Bones.push_back(b);
  }
  
  //ReorderBones(model);  // Ensure correct bone order. 
  BuildVertexGroups(model, associations);


  //_____________________________________________________________________[PROCESS ANIMATION DATA]
  // Adjusting the MDX sequences (global frame line, interleaved anim data) to ours.
  for (uint i = 0; i < sequences.size(); i ++) {
    Sequence seq = Sequence();
    strcpy(seq.Name, sequences[i].Name);
    seq.ID = i;
    seq.Length = sequences[i].IntervalEnd - sequences[i].IntervalStart;
    seq.Loop = sequences[i].Loop;

    // Now it's getting hard: We'll have to loop over each bone, its transformation 
    // sets (translation, rotation and scaling) and get all relevant frames.
    int start = sequences[i].IntervalStart;
    int end = sequences[i].IntervalEnd;    
    for (uint b = 0; b < bones.size(); b ++) {
      TransformationSet set = TransformationSet();    
      
      // Go over the TRS sets. Because they're basically the same, we use this generic loop.
      for (int setLoop = 0; setLoop < 3; setLoop ++) {
        MDX_AnimSet* animset;
        vector<TransformationDirective>* vec;
        if (setLoop == 0) { animset = bones[b].Translation; vec = &set.Translations; }
        if (setLoop == 1) { animset = bones[b].Rotation; vec = &set.Rotations; }
        if (setLoop == 2) { animset = bones[b].Scaling; vec = &set.Scalings; }
        if (animset == 0) continue;  // Skip on empty set, else loop over its entries.
        for (int s = 0; s < animset->Size; s ++) {
          int frame = animset->Time[s];
          if (frame < start) continue;   // Before start: Skip!
          if (frame > end) break;        // After interval: Exit loop.
          TransformationDirective dir = TransformationDirective();
          dir.Frame = frame - start;
          dir.X = animset->Values[s].X;
          dir.Y = animset->Values[s].Y;
          dir.Z = animset->Values[s].Z;
          if (setLoop == 1) dir.W = animset->Values[s].W;  //| 4th value (quaternion).
          vec->push_back(dir);                             //| We only have it on 2nd set!
        }
      }

      // Check for content. There's no need to insert empty sets (= bones not moved anyway).
      if (set.Translations.size() > 0 || set.Rotations.size() > 0 || set.Scalings.size() > 0) {
        for (uint i = 0; i < model->Bones.size(); i ++) {
          if (strcmp(model->Bones[i].Name, bones[b].Name) == 0) {
            seq.Transformations[&model->Bones[i]] = set;
            break;
          }
        }   
      }
    }
    model->Sequences.push_back(seq);
  }





  // Hey, we're done! Finally... Print parser results.
  printf("Parser results:\n"
    " - Meshes   : %d\n"
    " - Vertices : %d\n"
    " - Normals  : %d\n"
    " - UVs      : %d\n"
    " - Indices  : %d\n"
    " - Bones    : %d\n"
    " - Sequences: %d\n",
    model->Meshes.size(), model->Vertices.size(), model->Normals.size(), 
    model->UVs.size(), model->Indices.size(), model->Bones.size(), model->Sequences.size()
  );
  
  fclose(fp); // Close file stream.
  return model;
} 




/** Transform the MDX vertex groups into a bone weighting per vertex.
 * @param model Reference to the model.
 * @param ba Bone assignment vector, containing associations and the matrix list. */
static void BuildVertexGroups(Model* model, vector<MDX_BoneAssignment> ba) {
  int vIdx = 0;
  for (uint i = 0; i < ba.size(); i ++) {
    for (uint j = 0; j < ba[i].Associations.size(); j ++) {
      vector<BYTE> grp = ba[i].MatrixGroups[ba[i].Associations[j]];
      BoneWeight bw = BoneWeight();
      for (uint k = 0; k < 4; k ++) {
        if (k < grp.size()) bw.BoneIDs[k] = grp[k];
        else bw.BoneIDs[k] = 255;
      }

      // Calculate the weights.
      Float3 vPos = model->Vertices[vIdx++];
      float dist = 0.0f;
      for (uint k = 0; k < grp.size() && k < 4; k ++) {
        Float3 bPos = model->Bones[bw.BoneIDs[k]].Pivot;
        dist += bPos.Distance(vPos);
      }
      for (uint k = 0; k < grp.size() && k < 4; k ++) {
        Float3 bPos = model->Bones[bw.BoneIDs[k]].Pivot;
        bw.Factor[k] = bPos.Distance(vPos) / dist;
      }

      model->Weights.push_back(bw);
    }  
  } /*
  printf("Output:\n");
  for (uint i = 0; i < model->Weights.size(); i ++) {
    printf(" Weight %04d: {", i);
    BoneWeight w = model->Weights[i];
    for (uint j = 0; j < 4 && w.BoneIDs[j] != 255; j ++) {
      printf(" %02d ==> %4.2f ", w.BoneIDs[j], w.Factor[j]);
    }
    printf("}\n");
  }*/
}


/** Read the vertex-to-bone association groups.
 * @param fp Opened and positioned file pointer.
 * @return The bone assignment for the current geoset. */
static MDX_BoneAssignment ReadMDXBoneAssignment(FILE* fp) {
  DWORD dbuf, chunkSize;
  BYTE bbuf;
  MDX_BoneAssignment ba = MDX_BoneAssignment();

  fread(&dbuf, sizeof(DWORD), 1, fp);      // Skip 'GNDX'.
  fread(&chunkSize, sizeof(DWORD), 1, fp); //| Number of vertex groups. Should correlate 
  for (uint i = 0; i < chunkSize; i ++) {  //| with the number of vertices read.
    fread(&bbuf, sizeof(BYTE), 1, fp);
    ba.Associations.push_back(bbuf);
  }
  vector<DWORD> mtxGroups;                 // Vertex group component sizes.
  fread(&dbuf, sizeof(DWORD), 1, fp);      // Skip 'MTGC'.
  fread(&chunkSize, sizeof(DWORD), 1, fp); // Number of matrix groups.
  for (uint i = 0; i < chunkSize; i ++) {
    fread(&dbuf, sizeof(DWORD), 1, fp);
    mtxGroups.push_back(dbuf);
  }
  vector<DWORD> midxGroups;                // Bone association table.
  fread(&dbuf, sizeof(DWORD), 1, fp);      // Skip 'MATS'.
  fread(&chunkSize, sizeof(DWORD), 1, fp); // Number of matrix index groups.
  for (uint i = 0; i < chunkSize; i ++) {
    fread(&dbuf, sizeof(DWORD), 1, fp);
    midxGroups.push_back(dbuf);
  }


  // Process the read data.
  uint mOff = 0;
  for (uint i = 0; i < mtxGroups.size(); i ++) {
    vector<BYTE> indices;
    for (uint j = 0; j < mtxGroups[i]; j ++) {
      indices.push_back((BYTE) midxGroups[mOff++]);
    }
    ba.MatrixGroups.push_back(indices);
  } /*
  printf("Associations: %03d\n", ba.Associations.size());
  for (uint i = 0; i < ba.MatrixGroups.size(); i ++) {
    printf("Group [%02d]: { ", i);
    for (uint j = 0; j < ba.MatrixGroups[i].size(); j ++) {
      printf("%d", ba.MatrixGroups[i][j]);
      if (j < ba.MatrixGroups[i].size() - 1) printf(", ");
    }
    printf(" }\n");
  } */
  return ba;
}


/** Helper function to read a MDX node.
 * @param fp Opened file stream, correctly positioned.
 * @param inclSize Integer address to transfer total node size.
 * @return A structure containing the node data. */
static MDX_Bone ReadMDXNode(FILE* fp, DWORD& inclSize) {
  DWORD dbuf;
  MDX_Bone bone = MDX_Bone();
  fread(&inclSize,      sizeof(DWORD), 1, fp); // Read node structure total size.  
  fread(&bone.Name,     sizeof(char), 80, fp); // Read name of bone.
  fread(&bone.ID,       sizeof(DWORD), 1, fp); // Read 'ObjectID'.
  fread(&bone.ParentID, sizeof(DWORD), 1, fp); // Read 'ParentID'.
  fread(&dbuf,          sizeof(DWORD), 1, fp); // Skip 'Flags' (256 for bone, 0 for helper).

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
  return bone;
}


/** Bone reordering function (to match parents-before-childs criteria). 
 * @param model The model whose bones shall be reordered. */
static void ReorderBones(Model* model) {
  printf("Bone hierarchy check / reordering ... ");
  
  bool done = false;
  vector<Bone>& bones = model->Bones;
  int nrBones = model->Bones.size();

  while (!done) {
    done = true;
    int maxP = -1;  // Highest parent reference found. Update on iteration, stop on anomaly.
    for (int i = 0; i < nrBones; i ++) {
      if (bones[i].Parent < maxP) {
        
        // Find the right position to insert to.
        for (int j = 0; j < nrBones; j ++) {
          if (bones[j].Parent > bones[i].Parent) {
            Bone b = bones[i];
            bones.erase(bones.begin() + i);      // Remove from old position ...
            bones.insert(bones.begin() + j, b);  // and insert at new one.      
            for (int k = 0; k < nrBones; k ++) {
              if (bones[k].Parent == i) bones[k].Parent = j;  // Update parent refs to this node.
              else if (bones[k].Parent >= j && bones[k].Parent < i) { //| Update all other bones
                bones[k].Parent ++;                                   //| affected by the shift
              }                                                       //| in this [j, i] interval. 
            }
            break;
          }
        }
        done = false;
        break;
      }
      else maxP = model->Bones[i].Parent;
    }
  }
  printf("[done]\n");
}
