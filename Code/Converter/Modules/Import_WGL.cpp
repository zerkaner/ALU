#include <Converter/Converter.h>


/** String contains shortcut. */
bool IsLine(char* buffer, char* line) {
  return (strstr(buffer, line) != NULL);
}


/** Reads a WGL lump file (TojiCode).
 * @param filepath Path to a .wglvert or .wglmodel to load.
 * @return Loaded model structure. */
Model2* Converter::ReadWgl(const char* filepath) {
  printf("WGL parser loaded.\n");

  // First of all, get both files.
  char dir[256], file[80], name[40], ending[10], vertFile[256], modelFile[256];
  FileUtils::File_splitPath(filepath, dir, file);
  FileUtils::File_splitEnding(file, name, ending);
  sprintf(vertFile,  "%s/%s.%s", dir, name, "wglvert");
  sprintf(modelFile, "%s/%s.%s", dir, name, "wglmodel");
  FILE* fpV = FileUtils::File_open(vertFile, true);
  FILE* fpM = FileUtils::File_open(modelFile, false);
  if (fpV == NULL || fpM == NULL) {
    if (fpV != NULL) fclose(fpV);
    if (fpM != NULL) fclose(fpM);
    return NULL;
  }

  //__________________________________________________________________________[VERT FILE]

  DWORD dbuf = 0x00;    // "Read-in and discard" buffer.
  for (int i = 0; i < 3; i ++) fread(&dbuf, sizeof(DWORD), 1, fpV);
  DWORD nrLumps = dbuf;

  // Expect first lump be to vertex informtion, 2nd to be indices.
  DWORD vtxOffset, vtxLength, idxOffset, idxLength;
  fread(&dbuf,      sizeof(DWORD), 1, fpV);  // Skip 'vert'
  fread(&vtxOffset, sizeof(DWORD), 1, fpV);
  fread(&vtxLength, sizeof(DWORD), 1, fpV);
  fread(&dbuf,      sizeof(DWORD), 1, fpV);  // Skip 'indx'
  fread(&idxOffset, sizeof(DWORD), 1, fpV);
  fread(&idxLength, sizeof(DWORD), 1, fpV);

  Model2* model = new Model2();
  strcpy(model->Name, name); // Set the model name.
  model->Version = 2;        // Set version to '2' (latest).

  // Read vertices.
  fseek(fpV, vtxOffset, SEEK_SET);
  DWORD stride;
  fread(&dbuf, sizeof(DWORD), 1, fpV);    // Skip vertex flags.
  fread(&stride, sizeof(DWORD), 1, fpV);  // Read stride.
  uint nrElem = (vtxLength - 8) / stride; // Number of VNT elements.  
  for (uint i = 0; i < nrElem; i ++) {
    float in[3];
    fread(&in, sizeof(float), 3, fpV);
    model->Vertices.push_back(Float3(in[0], in[1], in[2]));
    fread(&in, sizeof(float), 2, fpV);
    model->UVs.push_back(Float2(in[0], in[1]));
    fread(&in, sizeof(float), 3, fpV);
    model->Normals.push_back(Float3(in[0], in[1], in[2]));
    int remaining = stride - 32;
    fseek(fpV, remaining, SEEK_CUR);
  }

  // Read the indices.
  fseek(fpV, idxOffset, SEEK_SET);
  for (uint i = 0; i < idxLength / 2; i ++) {
    WORD w;
    fread(&w, sizeof(WORD), 1, fpV);
    model->Indices.push_back(w);
  }


  //_________________________________________________________________________[MODEL FILE]

  char buffer[1024], bufferVal[768];
  int i1;

  Mesh2* mesh = NULL;
  Bone2* bone = NULL;
  int meshCounter = 0;
  bool readBones = false;

  // Main read-in loop. Runs over the entire file and evaluates the read line.
  while (fgets(buffer, 256, fpM) != NULL) {
    
    // The "defaultTexture" object has been found => new mesh!
    if (strstr(buffer, "defaultTexture") != NULL) {
      if (mesh != NULL) {
        model->Meshes.push_back(*mesh);
        meshCounter ++;
      }
      mesh = new Mesh2();
      FileUtils::JSON_getString(buffer, bufferVal);
      sprintf(mesh->ID, "Mesh %02d", meshCounter);
      strcpy(mesh->Texture, bufferVal);
      mesh->Enabled = true;
    }

    // Read submesh values.
    else if (IsLine(buffer, "indexOffset")) mesh->IndexOffset = FileUtils::JSON_getInteger(buffer);   
    else if (IsLine(buffer, "indexCount"))  mesh->IndexLength = FileUtils::JSON_getInteger(buffer);
    else if (IsLine(buffer, "boneOffset"))  mesh->BoneOffset  = FileUtils::JSON_getInteger(buffer);
    else if (IsLine(buffer, "boneCount"))   mesh->BoneCount   = FileUtils::JSON_getInteger(buffer);
    
    // Read bones.
    else if (strstr(buffer, "bones") != NULL) readBones = true;
    if (readBones) {
      
      // Got a new bone.
      if (IsLine(buffer, "name")) {
        if (bone != NULL) model->Bones.push_back(*bone);
        bone = new Bone2();
        FileUtils::JSON_getString(buffer, bufferVal);
        strcpy(bone->Name, bufferVal);
      }

      else if (IsLine(buffer, "parent")) bone->Parent = FileUtils::JSON_getInteger(buffer);
      else if (IsLine(buffer, "pos")) FileUtils::JSON_getFloatArray(buffer, (float*)&(bone->Position), i1);
      else if (IsLine(buffer, "rot")) FileUtils::JSON_getFloatArray(buffer, (float*)&(bone->Rotation), i1);
      else if (IsLine(buffer, "skinned")) bone->Skinned = FileUtils::JSON_getBool(buffer);
      else if (IsLine(buffer, "bindPoseMat")) FileUtils::JSON_getFloatArray(buffer, bone->BindPoseMat, i1);
    }
  }

  // Write last mesh and bone.
  if (mesh != NULL) model->Meshes.push_back(*mesh);
  if (bone != NULL) model->Bones.push_back(*bone);


  for (uint i = 0; i < model->Bones.size(); i ++) {
    printf("[%-18s] %2d (%9f, %9f, %9f)  |  (%9f, %9f, %9f, %9f)\n", 
      model->Bones[i].Name,
      model->Bones[i].Parent,
      model->Bones[i].Position.X,
      model->Bones[i].Position.Y,
      model->Bones[i].Position.Z,
      model->Bones[i].Rotation.X,
      model->Bones[i].Rotation.Y,
      model->Bones[i].Rotation.Z,
      model->Bones[i].Rotation.W
    );
  }


  fclose(fpV);
  fclose(fpM);
  return model;
}
