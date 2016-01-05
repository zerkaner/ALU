#pragma warning(disable: 4996)
#include "Model.h"
#include <stdio.h>
#include <stdlib.h>


/** Load a M4 binary file.
 * @param filepath Path to the file to load.
 * @return Pointer to the instantiated model. */
Model2* ModelUtils::Load(const char* filepath) {
  
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

  Model2* model = new Model2();  // Create model object. 
  model->Version = version;      // Set version number.

  // Read model name and vector sizes. Redimension vectors as needed.
  fread(&model->Name, sizeof(char), 80, fp);
  uint nrVtx, nrNor, nrUVs, nrIdx, nrMsh, nrBne, nrAni, nrTex;
  fread(&nrVtx, sizeof(uint), 1, fp);  model->Vertices.reserve(nrVtx);
  fread(&nrNor, sizeof(uint), 1, fp);  model->Normals.reserve(nrNor);
  fread(&nrUVs, sizeof(uint), 1, fp);  model->UVs.reserve(nrUVs);
  fread(&nrIdx, sizeof(uint), 1, fp);  model->Indices.reserve(nrIdx);
  fread(&nrMsh, sizeof(uint), 1, fp);  model->Meshes.reserve(nrMsh);
  fread(&nrBne, sizeof(uint), 1, fp);  model->Bones.reserve(nrBne);
  fread(&nrAni, sizeof(uint), 1, fp);  model->Animations.reserve(nrAni);
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
    Mesh2 mesh = Mesh2();
    fread(&mesh.ID,          sizeof(char), 32, fp);
    fread(&mesh.Enabled,     sizeof(bool),  1, fp);
    fread(&mesh.Texture,     sizeof(char), 80, fp);
    fread(&mesh.TextureIdx,  sizeof(short), 1, fp);
    fread(&mesh.IndexOffset, sizeof(DWORD), 1, fp);
    fread(&mesh.IndexLength, sizeof(DWORD), 1, fp);
    fread(&mesh.BoneOffset,  sizeof(DWORD), 1, fp);
    fread(&mesh.BoneCount,   sizeof(DWORD), 1, fp);
    model->Meshes.push_back(mesh);
  }

  // Read the bones.
  for (uint i = 0; i < nrBne; i ++) { /*TODO*/ }

  // Read the animation sequences.
  for (uint i = 0; i < nrAni; i ++) { /*TODO*/ }


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
void ModelUtils::Save(Model2* model, const char* savefile) {

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
  uint nrAni = model->Animations.size(); fwrite(&nrAni, sizeof(uint), 1, fp);
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
    fwrite(&model->Meshes[i].BoneOffset,  sizeof(DWORD), 1, fp);
    fwrite(&model->Meshes[i].BoneCount,   sizeof(DWORD), 1, fp);
  }

  // Write the bones.
  for (uint i = 0; i < nrBne; i ++) {
    //TODO Anwendungslogik hier implementieren, sobald verwendet!
  }

  // Write the animation sequences.
  for (uint i = 0; i < nrAni; i ++) {
    //TODO s.o.
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
void ModelUtils::ScaleModel(Model2* model, float factor) {
  printf("Scaling model by factor '%6.4f'.\n", factor);
  if (model == NULL) { printf("[Error] Model does not exist!\n"); return; }
  for (uint i = 0; i < model->Vertices.size(); i ++) {
    model->Vertices[i].X *= factor;
    model->Vertices[i].Y *= factor;
    model->Vertices[i].Z *= factor;
  }
  for (uint i = 0; i < model->Bones.size(); i ++) {
    model->Bones[i].Position.X *= factor;
    model->Bones[i].Position.Y *= factor;
    model->Bones[i].Position.Z *= factor;
  }
  for (uint i = 0; i < model->Sequences.size(); i ++) {
    //std::map<Bone2*, TransformationSet>::iterator iter;
    for (std::map<Bone2*, TransformationSet>::iterator iter = model->Sequences[i].Transformations.begin();
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
void ModelUtils::ScaleModelToExtent(Model2* model, char axis, float value) {
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
void ModelUtils::PrintDebug(Model2* model) {
  FILE* fp = fopen("dbgOut.txt", "w");

  // Bone hierarchy.
  fprintf(fp, "  Bones (%d):\n", model->Bones.size());
  for (uint i = 0; i < model->Bones.size(); i ++) {
    Bone2* bone = &model->Bones[i];
    fprintf(fp, 
      "  [%02d]  %-12s  Par> %02d  | Pos: %7.4f, %7.4f, %7.4f | Rot: %7.4f, %7.4f, %7.4f, %7.4f\n",
      i, bone->Name, bone->Parent, bone->Position.X, bone->Position.Y, bone->Position.Z,
      bone->Rotation.X, bone->Rotation.Y, bone->Rotation.Z, bone->Rotation.W  
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

    std::map<Bone2*, TransformationSet>::iterator iter;
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
