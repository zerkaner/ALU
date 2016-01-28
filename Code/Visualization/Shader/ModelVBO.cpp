#include <Data/Model.h>
#include "ExtLoader.h"

VertexBufferObject::VertexBufferObject(Model* mdl) {
  
  // Create the buffers.
  glGenBuffers(1, &VtxBuf);
  glGenBuffers(1, &NorBuf);
  glGenBuffers(1, &TexBuf);
  glGenBuffers(1, &IdxBuf);

  // Calculate array sizes.
  int nrVtx = mdl->Vertices.size() * 3;
  int nrNor = mdl->Normals.size() * 3;
  int nrTex = mdl->Textures.size() * 2;
  int nrIdx = mdl->Indices.size();

  // Create temporary (straight) arrays.
  float* vtx = new float[nrVtx];
  float* nor = new float[nrNor];
  float* tex = new float[nrTex];
  uint*  idx = new uint[nrIdx];

  // Copy Float3(), Float2()... stuff to these arrays.
  std::vector<Float3>& v = mdl->Vertices;
  std::vector<Float3>& n = mdl->Normals;
  std::vector<Float2>& t = mdl->UVs;
  for (int i = 0; i < nrVtx/3; i ++) { vtx[i*3]=v[i].X;  vtx[i*3+1]=v[i].Y;  vtx[i*3+2]=v[i].Z; }
  for (int i = 0; i < nrNor/3; i ++) { nor[i*3]=n[i].X;  nor[i*3+1]=n[i].Y;  nor[i*3+2]=n[i].Z; }
  for (int i = 0; i < nrTex/2; i ++) { tex[i*2]=t[i].X;  tex[i*2+1]=t[i].Y; }
  for (int i = 0; i < nrIdx;   i ++) { idx[i] = (uint) mdl->Indices[i]; }

  // Bind and fill the OpenGL buffers consecutively.
  glBindBuffer(GL_ARRAY_BUFFER, VtxBuf);
  glBufferData(GL_ARRAY_BUFFER, nrVtx * sizeof(float), vtx, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, NorBuf);
  glBufferData(GL_ARRAY_BUFFER, nrNor * sizeof(float), nor, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, TexBuf);
  glBufferData(GL_ARRAY_BUFFER, nrTex * sizeof(float), tex, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IdxBuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrIdx * sizeof(GLuint), idx, GL_STATIC_DRAW);

  // Remove the temporary arrays.
  delete vtx;
  delete nor;
  delete tex;
  delete idx;
}

void VertexBufferObject::Bind() {

}

VertexBufferObject::~VertexBufferObject() {

}
