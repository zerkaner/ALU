#pragma warning(disable: 4996)
#include "Model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** Load a M4 binary file.
 * @param filepath Path to the file to load.
 * @return Pointer to the instantiated model. */
Model* ModelUtils::Load(const char* filepath) {
  
  // Try to open the file.
  printf("Loading file '%s' ", filepath);
  FILE* fp = fopen(filepath, "rb");
  if (fp == NULL) { printf("[ERROR]\n"); return NULL; }

  // Echo file size.
  fseek(fp, 0L, SEEK_END);
  unsigned long bytes = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  printf("[%lu bytes]\n", bytes);
  
  // Read format identifier and version. Skip on mismatch.
  DWORD formatID;
  fread(&formatID, sizeof(DWORD), 1, fp);
  if (formatID != '4ULA') { 
  printf("[ERROR] File format not recognized!\n"); return NULL; 
  }
  WORD version;
  fread(&version, sizeof(WORD), 1, fp);
  if (version != 2) {
    printf("\n[ERROR] File version mismatch (expected '2', got '%d'!\n", version); 
    return NULL;
  }

  Model* model = new Model();  // Create model object. 
  model->Version = version;    // Set version number.

  // Read model name and vector sizes. Redimension vectors as needed.
  fread(&model->Name, sizeof(char), 80, fp);
  uint nrVtx, nrNor, nrUVs, nrIdx, nrMsh, nrBne, nrWgt, nrSeq, nrTex;
  fread(&nrVtx, sizeof(uint), 1, fp);  model->Vertices.reserve(nrVtx);
  fread(&nrNor, sizeof(uint), 1, fp);  model->Normals.reserve(nrNor);
  fread(&nrUVs, sizeof(uint), 1, fp);  model->UVs.reserve(nrUVs);
  fread(&nrIdx, sizeof(uint), 1, fp);  model->Indices.reserve(nrIdx);
  fread(&nrMsh, sizeof(uint), 1, fp);  model->Meshes.reserve(nrMsh);
  fread(&nrBne, sizeof(uint), 1, fp);  model->Bones.reserve(nrBne);
  fread(&nrSeq, sizeof(uint), 1, fp);  model->Sequences.reserve(nrSeq);
  fread(&nrWgt, sizeof(uint), 1, fp);  model->Weights.reserve(nrWgt);
  fread(&nrTex, sizeof(uint), 1, fp);  model->Textures.reserve(nrTex);
  
  // Read vertices, normals, texture vectors and indices.
  for (uint i = 0; i < nrVtx; i ++) {
    Float3 f = Float3();
    fread(&f, sizeof(Float3), 1, fp);
    model->Vertices.push_back(f);
  }
  for (uint i = 0; i < nrNor; i ++) {
    Float3 f = Float3();
    fread(&f, sizeof(Float3), 1, fp);
    model->Normals.push_back(f);
  }
  for (uint i = 0; i < nrUVs; i ++) {
    Float2 f = Float2();
    fread(&f, sizeof(Float2), 1, fp);
    model->UVs.push_back(f);
  }
  for (uint i = 0; i < nrIdx; i ++) {
    DWORD f;
    fread(&f, sizeof(DWORD), 1, fp);
    model->Indices.push_back(f);
  }

  // Read the meshes.
  for (uint i = 0; i < nrMsh; i ++) {
    Mesh mesh = Mesh();
    fread(&mesh.ID,          sizeof(char), 32, fp);
    fread(&mesh.Enabled,     sizeof(bool),  1, fp);
    fread(&mesh.Texture,     sizeof(char), 80, fp);
    fread(&mesh.TextureIdx,  sizeof(short), 1, fp);
    fread(&mesh.IndexOffset, sizeof(DWORD), 1, fp);
    fread(&mesh.IndexLength, sizeof(DWORD), 1, fp);
    fread(&mesh.Attached,    sizeof(bool),  1, fp);
    model->Meshes.push_back(mesh);
  }

  // Read the bones.
  for (uint i = 0; i < nrBne; i ++) { 
    Bone bone;
    fread(&bone.Name,   sizeof(char),  32, fp);
    fread(&bone.Parent, sizeof(int),    1, fp);
    fread(&bone.Pivot,  sizeof(Float3), 1, fp);
    model->Bones.push_back(bone);
  }

  // Read the animation sequences.
  for (uint i = 0; i < nrSeq; i ++) { 
    Sequence seq;
    fread(&seq.Name,   sizeof(char), 80, fp);
    fread(&seq.ID,     sizeof(int),   1, fp);
    fread(&seq.Length, sizeof(int),   1, fp);
    fread(&seq.Loop,   sizeof(bool),  1, fp);
    uint nrTSets;
    fread(&nrTSets, sizeof(uint), 1, fp); // Read size of Bone->TS map. 
    for (uint s = 0; s < nrTSets; s ++) { // Map read-in loop. 
      TransformationSet set;
      uint nt, nr, ns;
      char ref[32];
      fread(&ref, sizeof(char), 32, fp);  // Read bone name.
      fread(&nt, sizeof(uint), 1, fp);  set.Translations.reserve(nt);
      fread(&nr, sizeof(uint), 1, fp);  set.Rotations.reserve(nr);
      fread(&ns, sizeof(uint), 1, fp);  set.Scalings.reserve(ns);

      for (uint ti = 0; ti < nt; ti ++) { // Read the translations.
        TransformationDirective td;
        fread(&td.Frame, sizeof(int), 1, fp);
        fread(&td.X, sizeof(float), 1, fp);
        fread(&td.Y, sizeof(float), 1, fp);
        fread(&td.Z, sizeof(float), 1, fp);
        set.Translations.push_back(td);
      }

      for (uint ti = 0; ti < nr; ti ++) { // Read the rotations.
        TransformationDirective td;
        fread(&td.Frame, sizeof(int), 1, fp);
        fread(&td.X, sizeof(float), 1, fp);
        fread(&td.Y, sizeof(float), 1, fp);
        fread(&td.Z, sizeof(float), 1, fp);
        fread(&td.W, sizeof(float), 1, fp);
        set.Rotations.push_back(td);
      }

      for (uint ti = 0; ti < ns; ti ++) { // Read the scalings.
        TransformationDirective td;
        fread(&td.Frame, sizeof(int), 1, fp);
        fread(&td.X, sizeof(float), 1, fp);
        fread(&td.Y, sizeof(float), 1, fp);
        fread(&td.Z, sizeof(float), 1, fp);
        set.Scalings.push_back(td);
      }

      // Get the referenced bone index.
      Bone* bone = NULL;
      for (uint b = 0; b < model->Bones.size(); b ++) {
        if (strcmp(model->Bones[b].Name, ref) == 0) {
          bone = &model->Bones[b];
          break;
        }
      }
      seq.Transformations[bone] = set;
    }
    model->Sequences.push_back(seq);
  }


  // Read the weights.
  for (uint i = 0; i < nrWgt; i ++) {
    BoneWeight bw;
    BYTE w[4];    
    fread(&bw.BoneIDs, sizeof(BYTE), 4, fp);
    fread(&w,          sizeof(BYTE), 4, fp);
    for (int j = 0; j < 4; j ++) bw.Factor[j] = w[j]/255.0f;
    model->Weights.push_back(bw);
  }



  // Read the texture chunk.
  for (uint i = 0; i < nrTex; i ++) { 
    DWORD size;
    fread(&size, sizeof(DWORD), 1, fp);    
    BYTE* data = (BYTE*) calloc(size, sizeof(BYTE));
    fread(data, sizeof(BYTE), size, fp);
    SimpleTexture* texture = new SimpleTexture(data, size, "-internal-");
    texture->SetupGLTextureBuffer();
    model->Textures.push_back(texture);
  }
  return model;
}



/** Save a model in the native M4 file format.
 * @param model The model to save.
 * @param savefile Name of the save file. */
void ModelUtils::Save(Model* model, const char* savefile) {

  // Open writer stream.
  printf("Writing file '%s' ", savefile);
  FILE* fp = fopen(savefile, "wb");
  if (fp == NULL) { printf("[ERROR]\n"); return; }

  // Write format identifier, version and model name.
  DWORD formatID = '4ULA';
  WORD version = 2;
  fwrite(&formatID, sizeof(DWORD), 1, fp);
  fwrite(&version, sizeof(WORD), 1, fp);
  fwrite(&model->Name, sizeof(char), 80, fp);

  // Get and write array sizes.
  uint nrVtx = model->Vertices.size();   fwrite(&nrVtx, sizeof(uint), 1, fp);
  uint nrNor = model->Normals.size();    fwrite(&nrNor, sizeof(uint), 1, fp);
  uint nrUVs = model->UVs.size();        fwrite(&nrUVs, sizeof(uint), 1, fp);
  uint nrIdx = model->Indices.size();    fwrite(&nrIdx, sizeof(uint), 1, fp);
  uint nrMsh = model->Meshes.size();     fwrite(&nrMsh, sizeof(uint), 1, fp);
  uint nrBne = model->Bones.size();      fwrite(&nrBne, sizeof(uint), 1, fp);
  uint nrSeq = model->Sequences.size();  fwrite(&nrSeq, sizeof(uint), 1, fp);
  uint nrWgt = model->Weights.size();    fwrite(&nrWgt, sizeof(uint), 1, fp);
  uint nrTex = model->Textures.size();   fwrite(&nrTex, sizeof(uint), 1, fp);

  // Write vertices, normals, texture vectors and indices.
  for (uint i=0; i<nrVtx; i++) fwrite(&model->Vertices[i], sizeof(Float3), 1, fp);
  for (uint i=0; i<nrNor; i++) fwrite(&model->Normals[i],  sizeof(Float3), 1, fp);
  for (uint i=0; i<nrUVs; i++) fwrite(&model->UVs[i],      sizeof(Float2), 1, fp);
  for (uint i=0; i<nrIdx; i++) fwrite(&model->Indices[i],  sizeof(DWORD),  1, fp);

  // Write the meshes.
  for (uint i = 0; i < nrMsh; i ++) {
    fwrite(&model->Meshes[i].ID,          sizeof(char), 32, fp);
    fwrite(&model->Meshes[i].Enabled,     sizeof(bool),  1, fp);
    fwrite(&model->Meshes[i].Texture,     sizeof(char), 80, fp);
    fwrite(&model->Meshes[i].TextureIdx,  sizeof(short), 1, fp);
    fwrite(&model->Meshes[i].IndexOffset, sizeof(DWORD), 1, fp);
    fwrite(&model->Meshes[i].IndexLength, sizeof(DWORD), 1, fp);
    fwrite(&model->Meshes[i].Attached,    sizeof(bool),  1, fp);
  }

  // Write the bones.
  for (uint i = 0; i < nrBne; i ++) {
    fwrite(&model->Bones[i].Name,   sizeof(char),  32, fp);
    fwrite(&model->Bones[i].Parent, sizeof(int),    1, fp);
    fwrite(&model->Bones[i].Pivot,  sizeof(Float3), 1, fp);
  }

  // Write the animation sequences.
  for (uint i = 0; i < nrSeq; i ++) {
    fwrite(&model->Sequences[i].Name,   sizeof(char), 80, fp);
    fwrite(&model->Sequences[i].ID,     sizeof(int),   1, fp);
    fwrite(&model->Sequences[i].Length, sizeof(int),   1, fp);
    fwrite(&model->Sequences[i].Loop,   sizeof(bool),  1, fp);
    uint nrTSets = model->Sequences[i].Transformations.size();
    fwrite(&nrTSets, sizeof(uint), 1, fp);

    std::map<Bone*, TransformationSet>::iterator iter;
    Sequence& seq = model->Sequences[i];
    for (iter = seq.Transformations.begin(); iter != seq.Transformations.end(); iter ++) {
      std::vector<TransformationDirective>* t = &iter->second.Translations;
      std::vector<TransformationDirective>* r = &iter->second.Rotations;
      std::vector<TransformationDirective>* s = &iter->second.Scalings;
      fwrite(&iter->first->Name, sizeof(char), 32, fp);  // Write bone name. Change it to index!
      
      // Get and write the vector sizes. Needed for loading!
      uint nt = iter->second.Translations.size();
      uint nr = iter->second.Rotations.size();
      uint ns = iter->second.Scalings.size();
      fwrite(&nt, sizeof(uint), 1, fp);  
      fwrite(&nr, sizeof(uint), 1, fp);
      fwrite(&ns, sizeof(uint), 1, fp);

      for (uint ti = 0; ti < nt; ti ++) {
        TransformationDirective td = iter->second.Translations[ti];
        fwrite(&td.Frame, sizeof(int), 1, fp);
        fwrite(&td.X, sizeof(float), 1, fp);
        fwrite(&td.Y, sizeof(float), 1, fp);
        fwrite(&td.Z, sizeof(float), 1, fp);
      }

      for (uint ti = 0; ti < nr; ti ++) {
        TransformationDirective td = iter->second.Rotations[ti];
        fwrite(&td.Frame, sizeof(int), 1, fp);
        fwrite(&td.X, sizeof(float), 1, fp);
        fwrite(&td.Y, sizeof(float), 1, fp);
        fwrite(&td.Z, sizeof(float), 1, fp);
        fwrite(&td.W, sizeof(float), 1, fp);
      }

      for (uint ti = 0; ti < ns; ti ++) {
        TransformationDirective td = iter->second.Scalings[ti];
        fwrite(&td.Frame, sizeof(int), 1, fp);
        fwrite(&td.X, sizeof(float), 1, fp);
        fwrite(&td.Y, sizeof(float), 1, fp);
        fwrite(&td.Z, sizeof(float), 1, fp);
      }
    }
  }


  // Write the weights.
  for (uint i = 0; i < nrWgt; i ++) {
    fwrite(&model->Weights[i].BoneIDs, sizeof(BYTE), 4, fp);    
    BYTE w[4];
    for (int j = 0; j < 4; j ++) {
      float f = model->Weights[i].Factor[j];
      w[j] = (BYTE)(255*f);
    }
    fwrite(&w, sizeof(BYTE), 4, fp);
  }

  // Write the texture chunk.
  for (uint i = 0; i < nrTex; i ++) {
    DWORD size = model->Textures[i]->Size();
    BYTE* data = model->Textures[i]->Data();
    fwrite(&size, sizeof(DWORD), 1, fp);
    fwrite(data, sizeof(BYTE), size, fp);
  }

  // Close file stream and quit.
  printf("[%lu bytes out]\n", (DWORD) ftell(fp));
  fclose(fp);
}



/** Scales the model's positional values.
 * @param model The model to scale.
 * @param factor The scaling factor. */
void ModelUtils::ScaleModel(Model* model, float factor) {
  printf("Scaling model by factor '%6.4f'.\n", factor);
  if (model == NULL) { printf("[Error] Model does not exist!\n"); return; }
  for (uint i = 0; i < model->Vertices.size(); i ++) {
    model->Vertices[i].X *= factor;
    model->Vertices[i].Y *= factor;
    model->Vertices[i].Z *= factor;
  }
  for (uint i = 0; i < model->Bones.size(); i ++) {
    model->Bones[i].Pivot.X *= factor;
    model->Bones[i].Pivot.Y *= factor;
    model->Bones[i].Pivot.Z *= factor;
  }
  for (uint i = 0; i < model->Sequences.size(); i ++) {
    //std::map<Bone2*, TransformationSet>::iterator iter;
    for (std::map<Bone*, TransformationSet>::iterator iter = model->Sequences[i].Transformations.begin();
         iter != model->Sequences[i].Transformations.end(); 
         iter ++) {
      for (uint j = 0; j < iter->second.Translations.size(); j ++) {
        TransformationDirective* td = &iter->second.Translations[j];
        td->X *= factor;
        td->Y *= factor;
        td->Z *= factor;
      }
    }
  }
}


/** Internal helper function to get vector value based on axis. */
float getValue(Float3 vtx, char axis) {
  switch (axis) {
    case 'X': case 'x': return vtx.X;
    case 'Y': case 'y': return vtx.Y;
    case 'Z': case 'z': return vtx.Z;
    default: return -1.0f;
  }
}


/** Scales the model to a specific extent.
 * @param model The model to scale.
 * @param axis The axis to use as reference.
 * @param value The total extent on that axis. */
void ModelUtils::ScaleModelToExtent(Model* model, char axis, float value) {
  printf("Scaling model to extent '%f' on axis '%c'.\n", value, axis);

  // Parameter sanity check.
  if (value <= 0) {
    printf("Model scaling failed: Value is '%f' (must be greater than 0).\n", value);
    return;
  }
  if (!(axis == 'x' || axis == 'y' || axis == 'z' || axis == 'X' || axis == 'Y' || axis == 'Z')) {
    printf("Model scaling failed: Axis '%c' invalid (supported: x, y, z).\n", axis);
    return;
  }

  // Get minimal and maximal value.
  float minVal, maxVal;
  minVal = maxVal = getValue(model->Vertices[0], axis);
  for (uint i = 0; i < model->Vertices.size(); i ++) {
    float val = getValue(model->Vertices[i], axis);
    if (val > maxVal) maxVal = val;
    else if (val < minVal) minVal = val;
  }

  // Calculate maximal extents and perform scaling based on desired value. 
  float factor = value / (maxVal - minVal);
  ScaleModel(model, factor);
}


/** Write model properties as debug information to a text file.
 * @param model The model to output. */
void ModelUtils::PrintDebug(Model* model) {
  FILE* fp = fopen("dbgOut.txt", "w");

  // Bone hierarchy.
  fprintf(fp, "  Bones (%d):\n", model->Bones.size());
  for (uint i = 0; i < model->Bones.size(); i ++) {
    Bone* bone = &model->Bones[i];
    fprintf(fp, 
      "  [%02d]  %-24s  Par> %02d  | Pos: %7.4f, %7.4f, %7.4f\n",
      i, bone->Name, bone->Parent, bone->Pivot.X, bone->Pivot.Y, bone->Pivot.Z
    );
  }


  // Animation sequence output.
  fprintf(fp, "\n  Sequences (%d):", model->Sequences.size());
  for (uint i = 0; i < model->Sequences.size(); i ++) {
    Sequence* seq = &model->Sequences[i];
    fprintf(fp,
      "\n"
      "  [%02d]\n"
      "  - Name:   %s\n"
      "  - ID:     %d\n"
      "  - Length: %d\n"
      "  - Loop:   %s\n"
      "  - Transformations (%d sets):\n",

      i, seq->Name, seq->ID, seq->Length, seq->Loop? "true" : "false", 
      seq->Transformations.size()
    );

    std::map<Bone*, TransformationSet>::iterator iter;
    for (iter = seq->Transformations.begin(); iter != seq->Transformations.end(); iter ++) {    
      std::vector<TransformationDirective>* t = &iter->second.Translations;
      std::vector<TransformationDirective>* r = &iter->second.Rotations;
      std::vector<TransformationDirective>* s = &iter->second.Scalings;
      
      fprintf(fp, "    ['%s']\n", iter->first->Name); // Bone name. 
      fprintf(fp, "    - T: ");                       // Translation output.    
      for (uint ti = 0; ti < iter->second.Translations.size(); ti ++) {
        TransformationDirective td = iter->second.Translations[ti];
        fprintf(fp, "{%d: (%f, %f, %f)}, ", td.Frame, td.X, td.Y, td.Z);
      } 

      fprintf(fp, "\n    - R: ");                    // Rotation output.
      for (uint ti = 0; ti < iter->second.Rotations.size(); ti ++) {
        TransformationDirective td = iter->second.Rotations[ti];
        fprintf(fp, "{%d: (%f, %f, %f, %f)}, ", td.Frame, td.X, td.Y, td.Z, td.W);
      }
      
      fprintf(fp, "\n    - S: ");                    // Scaling output.
      for (uint ti = 0; ti < iter->second.Scalings.size(); ti ++) {
        TransformationDirective td = iter->second.Scalings[ti];
        fprintf(fp, "{%d: (%f, %f, %f)}, ", td.Frame, td.X, td.Y, td.Z);
      }

      fprintf(fp, "\n");
    }
  }

  fclose(fp);
}
