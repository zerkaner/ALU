#include <Converter/Converter2.h>


Model2* Converter2::ReadWgl(FILE* fp) {
  printf("WGL parser loaded.\n");

  DWORD dbuf = 0x00;    // "Read-in and discard" buffer.
  for (int i = 0; i < 3; i ++) fread(&dbuf, sizeof(DWORD), 1, fp);
  DWORD nrLumps = dbuf;

  // Expect first lump be to vertex informtion, 2nd to be indices.
  DWORD vtxOffset, vtxLength, idxOffset, idxLength;
  fread(&dbuf, sizeof(DWORD), 1, fp);  // Skip 'vert'
  fread(&vtxOffset, sizeof(DWORD), 1, fp);
  fread(&vtxLength, sizeof(DWORD), 1, fp);
  fread(&dbuf, sizeof(DWORD), 1, fp);  // Skip 'indx'
  fread(&idxOffset, sizeof(DWORD), 1, fp);
  fread(&idxLength, sizeof(DWORD), 1, fp);

  Model2* model = new Model2();

  // Read vertices.
  fseek(fp, vtxOffset, SEEK_SET);
  DWORD stride;
  fread(&dbuf, sizeof(DWORD), 1, fp);     // Skip vertex flags.
  fread(&stride, sizeof(DWORD), 1, fp);   // Read stride.
  uint nrElem = (vtxLength - 8) / stride; // Number of VNT elements.  
  for (uint i = 0; i < nrElem; i ++) {
    float in[3];
    fread(&in, sizeof(float), 3, fp);
    model->Vertices.push_back(Float3(in[0], in[1], in[2]));
    fread(&in, sizeof(float), 2, fp);
    model->UVs.push_back(Float2(in[0], in[1]));
    fread(&in, sizeof(float), 3, fp);
    model->Normals.push_back(Float3(in[0], in[1], in[2]));
    int remaining = stride - 32;
    fseek(fp, remaining, SEEK_CUR);
  }

  // Read the indices.
  fseek(fp, idxOffset, SEEK_SET);
  for (uint i = 0; i < idxLength / 2; i ++) {
    WORD w;
    fread(&w, sizeof(WORD), 1, fp);
    model->Indices.push_back(w);
  }

  Mesh2 mesh = Mesh2();
  strcpy(mesh.ID, "Mesh-01");
  strcpy(mesh.Texture, "RobotArm.png");
  mesh.Enabled = true;
  mesh.IndexOffset = 0;
  mesh.IndexLength = model->Indices.size();
  mesh.BoneOffset = 0;
  mesh.BoneCount = 0;
  model->Meshes.push_back(mesh);

  return model;
}
