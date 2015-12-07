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
  printf("[%lu bytes] ", bytes);
  
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
  uint nrVtx, nrNor, nrUVs, nrIdx, nrMsh, nrBne, nrAni;
  fread(&nrVtx, sizeof(uint), 1, fp);  model->Vertices.reserve(nrVtx);
  fread(&nrNor, sizeof(uint), 1, fp);  model->Normals.reserve(nrNor);
  fread(&nrUVs, sizeof(uint), 1, fp);  model->UVs.reserve(nrUVs);
  fread(&nrIdx, sizeof(uint), 1, fp);  model->Indices.reserve(nrIdx);
  fread(&nrMsh, sizeof(uint), 1, fp);  model->Meshes.reserve(nrMsh);
  fread(&nrBne, sizeof(uint), 1, fp);  model->Bones.reserve(nrBne);
  fread(&nrAni, sizeof(uint), 1, fp);  model->Animations.reserve(nrAni);
  
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

  // Close file stream and quit.
  printf("[%lu bytes out]\n", (DWORD) ftell(fp));
  fclose(fp);
}



/** Scales the model's vertices.
 * @param model The model to scale.
 * @param factor The scaling factor. */
void ModelUtils::ScaleModel(Model2* model, float factor) {
  printf("Scaling model by factor '%6.4f'.\n", factor);
  for (uint i = 0; i < model->Vertices.size(); i ++) {
    model->Vertices[i].X *= factor;
    model->Vertices[i].Y *= factor;
    model->Vertices[i].Z *= factor;
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
